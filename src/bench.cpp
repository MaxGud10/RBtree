#include <iostream>
#include <cstdint>
#include <set>
#include <chrono>
#include <string>
#include <string_view>

#include "red_black_tree.hpp"

using Clock = std::chrono::steady_clock;
using ns    = std::chrono::nanoseconds;
using us    = std::chrono::microseconds;

static long long get_bench_batch_arg(int argc, char** argv, long long def_val)
{
    for (int i = 1; i < argc; ++i) 
    {
        std::string_view a = argv[i];
        if (a.rfind("--bench-batch=", 0) == 0)
            return std::stoll(std::string(a.substr(14)));
    }

    return def_val;
}

static inline int64_t range_queries_set(const std::set<int64_t>& set, int64_t a, int64_t b)
{
    if (b < a) 
        return 0;
    auto first = set.lower_bound(a);
    auto last  = set.upper_bound(b);

    return std::distance(first, last);
}

int main(int argc, char** argv)
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const long long batch_sz = std::max(1LL, get_bench_batch_arg(argc, argv, 2000));

    Tree::Red_black_tree<int64_t> tree;
#ifdef SET_MODE_ENABLED
    std::set<int64_t> set;
#endif

    ns our_insert_time{0}, our_query_time{0};
    ns set_insert_time{0}, set_query_time{0};

    std::size_t our_ins_in_batch = 0, our_qry_in_batch = 0;
    std::size_t ins_cnt = 0, qry_cnt = 0;

#ifdef SET_MODE_ENABLED
    std::size_t set_ins_in_batch = 0, set_qry_in_batch = 0;
#endif

    Clock::time_point our_ins_t0{}, our_qry_t0{};
#ifdef SET_MODE_ENABLED
    Clock::time_point set_ins_t0{}, set_qry_t0{};
#endif

    auto start_if_first = [](std::size_t in_batch, Clock::time_point& t0) 
    {
        if (in_batch == 0) t0 = Clock::now();
    };
    auto close_if_full = [&](std::size_t& in_batch, ns& total, Clock::time_point t0) 
    {
        if (++in_batch == static_cast<std::size_t>(batch_sz)) 
        {
            total += std::chrono::duration_cast<ns>(Clock::now() - t0);
            in_batch = 0;
        }
    };
    auto flush_tail = [&](std::size_t in_batch, ns& total, Clock::time_point t0) 
    {
        if (in_batch > 0) total += std::chrono::duration_cast<ns>(Clock::now() - t0);
    };

    char    mode;
    int64_t key = 0;
    int64_t a   = 0;
    int64_t b   = 0;

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
                tree.insert_elem(a);
                close_if_full(our_ins_in_batch, our_insert_time, our_ins_t0);

#ifdef SET_MODE_ENABLED
                start_if_first(set_ins_in_batch, set_ins_t0);
                set.insert(key);
                close_if_full(set_ins_in_batch, set_insert_time, set_ins_t0);
#endif

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
                close_if_full(our_qry_in_batch, our_query_time, our_qry_t0);

#ifdef SET_MODE_ENABLED
                start_if_first(set_qry_in_batch, set_qry_t0);
                const auto check = range_queries_set(set, a, b);
                close_if_full(set_qry_in_batch, set_query_time, set_qry_t0);
                if (ans != check) 
                {
                    std::cerr << "MISMATCH: [" << a << ' ' << b
                              << "] our=" << ans << " set=" << check << '\n';
                }
#endif

                std::cout << ans << ' ';
                printed_any_answer = true;

                ++qry_cnt;
                break;
            }

            default:
                std::cerr << "ERROR: unknown mode: '" << mode << "'\n";
                return 1;
        }
    }

    if (printed_any_answer) 
        std::cout << '\n';

    flush_tail(our_ins_in_batch, our_insert_time, our_ins_t0);
    flush_tail(our_qry_in_batch, our_query_time, our_qry_t0);
    
#ifdef SET_MODE_ENABLED
    flush_tail(set_ins_in_batch, set_insert_time, set_ins_t0);
    flush_tail(set_qry_in_batch, set_query_time, set_qry_t0);
#endif

    const auto us_our_ins = std::chrono::duration_cast<us>(our_insert_time).count();
    const auto us_our_qry = std::chrono::duration_cast<us>(our_query_time) .count();

#ifdef SET_MODE_ENABLED
    const auto us_set_ins = std::chrono::duration_cast<us>(set_insert_time).count();
    const auto us_set_qry = std::chrono::duration_cast<us>(set_query_time) .count();
#else
    const long long us_set_ins = 0;
    const long long us_set_qry = 0;
#endif

    const double ns_per_ins_our = ins_cnt ? double(our_insert_time.count()) / double(ins_cnt) : 0.0;
    const double ns_per_qry_our = qry_cnt ? double(our_query_time.count() ) / double(qry_cnt) : 0.0;

#ifdef SET_MODE_ENABLED
    const double ns_per_ins_set = ins_cnt ? double(set_insert_time.count()) / double(ins_cnt) : 0.0;
    const double ns_per_qry_set = qry_cnt ? double(set_query_time.count() ) / double(qry_cnt) : 0.0;
#else
    const double ns_per_ins_set = 0.0;
    const double ns_per_qry_set = 0.0;
#endif

    std::cerr
        << "[BENCH]\n"
        << "batch      : ~" << batch_sz << "\n"
        << "insert ops : " << ins_cnt << "\n"
        << "query  ops : " << qry_cnt << "\n\n"
        << "Our tree:\n"
        << "  insert: " << us_our_ins << " us total  (" << ns_per_ins_our << " ns/op)\n"
        << "  query : " << us_our_qry << " us total  (" << ns_per_qry_our << " ns/op)\n\n"
        << "std::set:\n"
        << "  insert: " << us_set_ins << " us total  (" << ns_per_ins_set << " ns/op)\n"
        << "  query : " << us_set_qry << " us total  (" << ns_per_qry_set << " ns/op)\n";

    return 0;
}
