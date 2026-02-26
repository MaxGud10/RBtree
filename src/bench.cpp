#include <iostream>
#include <cstdint>
#include <set>
#include <chrono>
#include <algorithm>

#include "cxxopts.hpp"
#include "red_black_tree.hpp"
#include "driver.hpp"

using TreeT = Tree::Red_black_tree<int64_t>;
using Clock = std::chrono::steady_clock;
using ns    = std::chrono::nanoseconds;
using us    = std::chrono::microseconds;

static long long get_bench_batch_arg(int argc, char** argv, long long def_val)
{
    cxxopts::Options options("rb_tree_bench", "RB-tree benchmark");

    options.add_options()
        ("bench-batch",
         "Batch size for benchmark",
         cxxopts::value<long long>()->default_value(std::to_string(def_val)));

    auto result = options.parse(argc, argv);

    return result["bench-batch"].as<long long>();
}

struct Batch_timer
{
    std::size_t in_batch_ = 0;
    ns total_{0};
    Clock::time_point t0_{};

    void start()
    {
        if (in_batch_ == 0)
            t0_ = Clock::now();
    }

    void stop(std::size_t batch_sz)
    {
        if (++in_batch_ == batch_sz)
        {
            total_ += std::chrono::duration_cast<ns>(Clock::now() - t0_);
            in_batch_ = 0;
        }
    }

    void flush()
    {
        if (in_batch_ > 0)
        {
            total_ += std::chrono::duration_cast<ns>(Clock::now() - t0_);
            in_batch_ = 0;
        }
    }
};

struct Bench_policy
{
    explicit Bench_policy(std::size_t batch_sz)
        : batch_sz_(batch_sz) {}

    std::size_t batch_sz_;

    std::set<int64_t> ref_;

    std::size_t ins_cnt_ = 0;
    std::size_t qry_cnt_ = 0;

    Batch_timer our_ins_;
    Batch_timer our_qry_;
    Batch_timer set_ins_;
    Batch_timer set_qry_;

    void insert(TreeT &tree, int64_t key)
    {
        our_ins_.start();
        tree.insert_elem(key);
        our_ins_.stop(batch_sz_);

        if constexpr (Driver::kVerifyWithSet)
        {
            set_ins_.start();
            ref_.insert(key);
            set_ins_.stop(batch_sz_);
        }

        ++ins_cnt_;
    }

    int64_t query(TreeT &tree, int64_t a, int64_t b)
    {
        our_qry_.start();
        const auto ans = tree.range_queries(a, b);
        our_qry_.stop(batch_sz_);

        if constexpr (Driver::kVerifyWithSet)
        {
            set_qry_.start();
            auto first = ref_.lower_bound(a);
            auto last  = ref_.upper_bound(b);
            const auto check = std::distance(first, last);
            set_qry_.stop(batch_sz_);

            if (check != ans)
            {
                std::cerr << "MISMATCH: [" << a << ' ' << b
                          << "] our=" << ans
                          << " set=" << check << '\n';
            }
        }

        ++qry_cnt_;
        return ans;
    }

    void handle_answer(int64_t, int64_t, int64_t){}

    void finalize()
    {
        our_ins_.flush();
        our_qry_.flush();

        if constexpr (Driver::kVerifyWithSet)
        {
            set_ins_.flush();
            set_qry_.flush();
        }

        const auto us_our_ins =
            std::chrono::duration_cast<us>(our_ins_.total_).count();

        const auto us_our_qry =
            std::chrono::duration_cast<us>(our_qry_.total_).count();

        std::cerr
            << "[BENCH]\n"
            << "batch      : " << batch_sz_ << "\n"
            << "insert ops : " << ins_cnt_  << "\n"
            << "query  ops : " << qry_cnt_  << "\n\n"
            << "Our tree:\n"
            << "  insert: " << us_our_ins << " us total\n"
            << "  query : " << us_our_qry << " us total\n";

        if constexpr (Driver::kVerifyWithSet)
        {
            const auto us_set_ins =
                std::chrono::duration_cast<us>(set_ins_.total_).count();

            const auto us_set_qry =
                std::chrono::duration_cast<us>(set_qry_.total_).count();

            std::cerr
                << "\nstd::set:\n"
                << "  insert: " << us_set_ins << " us total\n"
                << "  query : " << us_set_qry << " us total\n";
        }
    }
};

int main(int argc, char** argv)
{
    const long long raw =
        get_bench_batch_arg(argc, argv, 2000);

    const std::size_t batch_sz =
        static_cast<std::size_t>(std::max(1LL, raw));

    TreeT tree;
    Bench_policy policy(batch_sz);

    return Driver::run(tree, policy);
}
