#include <iostream>
#include <cstdint>
#include <set>
#include <chrono>
#include "cxxopts.hpp"
#include <string>
#include <string_view>

#include "red_black_tree.hpp"

using Clock = std::chrono::steady_clock;
using ns    = std::chrono::nanoseconds;
using us    = std::chrono::microseconds;

#ifdef SET_MODE_ENABLED
constexpr bool kVerifyWithSet = true;
#else
constexpr bool kVerifyWithSet = false;
#endif

static long long get_bench_batch_arg(int argc, char** argv, long long def_val)
{
    cxxopts::Options options("rb_tree_bench", "RB-tree benchmark");

    options.add_options()
        ("bench-batch", "Batch size for benchmark",
             cxxopts::value<long long>()->default_value(std::to_string(def_val)));

    auto result = options.parse(argc, argv);

    return result["bench-batch"].as<long long>();
}

static inline int64_t range_queries_set(const std::set<int64_t> &set, int64_t a, int64_t b);
static inline void    start_if_first   (std::size_t in_batch, Clock::time_point &t0);
static inline void    close_if_full    (std::size_t &in_batch, ns &total,  Clock::time_point t0, std::size_t batch_sz);
static inline void    flush_tail       (std::size_t in_batch,  ns &total,  Clock::time_point t0);


static inline void bench_insert_set      (      std::set<int64_t> &set, int64_t key, std::size_t &in_batch, ns &total, Clock::time_point &t0, std::size_t batch_sz);
static inline void verify_and_bench_query(const std::set<int64_t> &set, int64_t a, int64_t b, int64_t ans, std::size_t &in_batch, ns &total, Clock::time_point &t0, std::size_t batch_sz);


int main(int argc, char** argv)
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const long long   batch_sz_raw = get_bench_batch_arg(argc, argv, 2000);
    const std::size_t batch_sz     = static_cast<std::size_t>(std::max(1LL, batch_sz_raw));

    Tree::Red_black_tree<int64_t> tree;
    
    std::set<int64_t> set;

    ns our_insert_time{0}, our_query_time{0};
    ns set_insert_time{0}, set_query_time{0};

    std::size_t our_ins_in_batch = 0, our_qry_in_batch = 0;
    std::size_t ins_cnt          = 0, qry_cnt          = 0;

    std::size_t set_ins_in_batch = 0, set_qry_in_batch = 0;


    Clock::time_point our_ins_t0{}, our_qry_t0{};
    Clock::time_point set_ins_t0{}, set_qry_t0{};

    char mode;
    int64_t key = 0, a = 0, b = 0;

    bool printed_any_answer = false;

    while (std::cin >> mode)
    {
        switch (mode)
        {
            case 'k': 
            {
                if (!(std::cin >> key))
                {
                    std::cerr << "ERROR: expected key after 'k'\n";
                    return 1;
                }

                start_if_first(our_ins_in_batch, our_ins_t0);
                tree.insert_elem(key);
                close_if_full(our_ins_in_batch,  our_insert_time, our_ins_t0, batch_sz);

                if constexpr (kVerifyWithSet)
                {
                    bench_insert_set(set, key,
                                     set_ins_in_batch, set_insert_time, set_ins_t0,
                                     batch_sz);
                }

                ++ins_cnt;
                break;
            }

            case 'q': 
            {
                if (!(std::cin >> a >> b))
                {
                    std::cerr << "ERROR: expected two numbers after 'q'\n";
                    return 1;
                }

                start_if_first(our_qry_in_batch, our_qry_t0);
                const auto ans = tree.range_queries(a, b);
                close_if_full(our_qry_in_batch, our_query_time, our_qry_t0, batch_sz);

                if constexpr (kVerifyWithSet)
                {
                    verify_and_bench_query(set, a, b, ans,
                                           set_qry_in_batch, set_query_time, set_qry_t0,
                                           batch_sz);
                }

                std::cout << ans << ' ';
                printed_any_answer = true;

                ++qry_cnt;
                break;
            }

            default:
                std::cerr << "ERROR: unknown mode '" << mode << "'\n";
                return 1;
        }
    }

    if (printed_any_answer)
        std::cout << '\n';

    flush_tail(our_ins_in_batch, our_insert_time, our_ins_t0);
    flush_tail(our_qry_in_batch, our_query_time,  our_qry_t0);

    if constexpr (kVerifyWithSet)
    {
        flush_tail(set_ins_in_batch, set_insert_time, set_ins_t0);
        flush_tail(set_qry_in_batch, set_query_time,  set_qry_t0);
    }

    const auto us_our_ins = std::chrono::duration_cast<us>(our_insert_time).count();
    const auto us_our_qry = std::chrono::duration_cast<us>(our_query_time ).count();

    const long long us_set_ins = kVerifyWithSet ? std::chrono::duration_cast<us>(set_insert_time).count() : 0;
    const long long us_set_qry = kVerifyWithSet ? std::chrono::duration_cast<us>(set_query_time ).count() : 0;

    std::cerr
        << "[BENCH]\n"
        << "batch      : " << batch_sz << "\n"
        << "insert ops : " << ins_cnt  << "\n"
        << "query  ops : " << qry_cnt  << "\n\n"
        << "Our tree:\n"
        << "  insert: " << us_our_ins << " us total  \n"
        << "  query : " << us_our_qry << " us total  \n\n";

    if constexpr (kVerifyWithSet)
    {
        std::cerr
            << "std::set:\n"
            << "  insert: " << us_set_ins << " us total\n"
            << "  query : " << us_set_qry << " us total\n";
    }

    return 0;
}


static inline int64_t range_queries_set(const std::set<int64_t> &set, int64_t a, int64_t b)
{
    if (b < a) 
        return 0;

    auto first = set.lower_bound(a);
    auto last  = set.upper_bound(b);

    return std::distance(first, last);
}

static inline void start_if_first(std::size_t in_batch, Clock::time_point &t0)
{
    if (in_batch == 0)
        t0 = Clock::now();
}

static inline void close_if_full(std::size_t &in_batch, ns &total, Clock::time_point t0, std::size_t batch_sz)
{
    if (++in_batch == batch_sz)
    {
        total += std::chrono::duration_cast<ns>(Clock::now() - t0);
        in_batch = 0;
    }
}

static inline void flush_tail(std::size_t in_batch, ns &total, Clock::time_point t0)
{
    if (in_batch > 0)
        total += std::chrono::duration_cast<ns>(Clock::now() - t0);
}

// вызываются только при if constexpr(kVerifyWithSet)
static inline void bench_insert_set(std::set<int64_t> &set,int64_t key, std::size_t &in_batch, ns &total, Clock::time_point &t0, std::size_t batch_sz)
{
    start_if_first(in_batch, t0);
    set.insert(key);
    close_if_full(in_batch, total, t0, batch_sz);
}

static inline void verify_and_bench_query(const std::set<int64_t> &set, int64_t a, int64_t b, int64_t ans, std::size_t &in_batch, ns &total, Clock::time_point &t0, std::size_t batch_sz)
{
    start_if_first(in_batch, t0);
    const auto check = range_queries_set(set, a, b);
    close_if_full(in_batch, total, t0, batch_sz);

    if (ans != check)
    {
        std::cerr << "MISMATCH: [" << a << ' ' << b
                  << "] our=" << ans
                  << " set=" << check << '\n';
    }
}
