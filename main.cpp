#include <iostream>
#include <cstdint>
#include <set>
#include <chrono>

#include "red_black_tree.hpp"
#include "graphic_dump.hpp"


enum class RunMode { AlgoOnly, VerifyWithSet, BenchBoth };

#ifndef SET_MODE_ENABLED
constexpr RunMode kRunMode = RunMode::AlgoOnly;
#else
  #define STR_HELPER(x) #x
  #define STR(x) STR_HELPER(x)

  static constexpr const char* kModeLiteral = STR(CUSTOM_SET_VALUE);
  constexpr RunMode kRunMode =
      ((kModeLiteral[0]=='B' && kModeLiteral[1]=='E' && kModeLiteral[2]=='N' && kModeLiteral[3]=='C' && kModeLiteral[4]=='H')
    || (kModeLiteral[0]=='T' && kModeLiteral[1]=='I' && kModeLiteral[2]=='M' && kModeLiteral[3]=='E'))
          ? RunMode::BenchBoth
          : RunMode::VerifyWithSet;
#endif


static inline int64_t range_queries_set(const std::set<int64_t>& s, int64_t a, int64_t b) 
{
    if (b <= a) return 0;          
    auto first = s.lower_bound(a);
    auto last  = s.upper_bound(b);
    return std::distance(first, last);
}

int main() 
{
    using Clock = std::chrono::steady_clock;
    using ns    = std::chrono::nanoseconds;

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    RangeQueries::Range_queries<int64_t> range_quer;

    #if defined(SET_MODE_ENABLED)
        std::set<int64_t> set;   
    #endif


    ns our_insert_time{0};
    ns our_query_time{0};
    ns set_insert_time{0};
    ns set_query_time{0};

    std::size_t ins_cnt = 0;
    std::size_t qry_cnt = 0;

    char mode;
    uint64_t key = 0;
    int64_t a = 0, b = 0;

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

                if (kRunMode == RunMode::BenchBoth) 
                {
                    auto t0 = Clock::now();
                    range_quer.rb_tree.insert_elem(static_cast<int64_t>(key));
                    auto t1 = Clock::now();
                    our_insert_time += std::chrono::duration_cast<ns>(t1 - t0);

                    #if defined(SET_MODE_ENABLED)
                        auto t2 = Clock::now();
                        set.insert(static_cast<int64_t>(key));
                        auto t3 = Clock::now();
                        set_insert_time += std::chrono::duration_cast<ns>(t3 - t2);
                    #endif
                } 

                else 
                {
                    range_quer.rb_tree.insert_elem(static_cast<int64_t>(key));
                    #ifdef SET_MODE_ENABLED
                        set.insert(static_cast<int64_t>(key));
                    #endif
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

                if (kRunMode == RunMode::BenchBoth) 
                {
                    auto t0 = Clock::now();
                    const auto ans = range_quer.find_range_elements(a, b);
                    auto t1 = Clock::now();
                    our_query_time += std::chrono::duration_cast<ns>(t1 - t0);

                    #if defined(SET_MODE_ENABLED)
                        auto t2 = Clock::now();
                        const auto ref_ans = range_queries_set(set, a, b);
                        auto t3 = Clock::now();
                        set_query_time += std::chrono::duration_cast<ns>(t3 - t2);
                    #else
                    const auto ref_ans = ans; // чтобы не плодить ifdef в печати mismatch
                    #endif

                    std::cout << ans << ' ';
                    printed_any_answer = true;

                    #if defined(SET_MODE_ENABLED)
                        if (ans != ref_ans) {
                            std::cerr << "MISMATCH: [" << a << ' ' << b
                                    << "] our=" << ans << " set=" << ref_ans << '\n';
                        }
                    #endif
                } 

                else 
                {
                    const auto ans = range_quer.find_range_elements(a, b);
                    std::cout << ans << ' ';
                    printed_any_answer = true;
                    #ifdef SET_MODE_ENABLED
                        if (kRunMode == RunMode::VerifyWithSet) {
                            const auto ref_ans = range_queries_set(set, a, b);
                            if (ref_ans != ans) {
                                std::cerr << "DBG set=" << ref_ans
                                        << " tree="   << ans
                                        << " for q "  << a << ' ' << b << '\n';
                            }
                        }
                    #endif
                }

                ++qry_cnt;
                break;
            }

            default:
                std::cerr << "ERROR: unknown mode: '" << mode << "'\n";
                return 1;
        }
    }

    if (printed_any_answer) std::cout << '\n';

    if (kRunMode == RunMode::BenchBoth) 
    {
        using us = std::chrono::microseconds;

        const auto us_our_ins  = std::chrono::duration_cast<us>(our_insert_time).count();
        const auto us_our_qry  = std::chrono::duration_cast<us>(our_query_time).count();
        #if defined(SET_MODE_ENABLED)
            const auto us_set_ins  = std::chrono::duration_cast<us>(set_insert_time).count();
            const auto us_set_qry  = std::chrono::duration_cast<us>(set_query_time).count();
        #else
        const long long us_set_ins = 0;
        const long long us_set_qry = 0;
        #endif

        const double ns_per_ins_our = ins_cnt ? static_cast<double>(our_insert_time.count()) / static_cast<double>(ins_cnt) : 0.0;
        const double ns_per_qry_our = qry_cnt ? static_cast<double>(our_query_time.count()) / static_cast<double>(qry_cnt) : 0.0;
        #if defined(SET_MODE_ENABLED)
            const double ns_per_ins_set = ins_cnt ? static_cast<double>(set_insert_time.count()) / static_cast<double>(ins_cnt) : 0.0;
            const double ns_per_qry_set = qry_cnt ? static_cast<double>(set_query_time.count()) / static_cast<double>(qry_cnt) : 0.0;
        #else
        const double ns_per_ins_set = 0.0;
        const double ns_per_qry_set = 0.0;
        #endif

        std::cerr
            << "[BENCH]\n"
            << "  our.insert: " << us_our_ins << " us total, " << ns_per_ins_our << " ns/op (" << ins_cnt << " ops)\n"
            << "  our.query : " << us_our_qry << " us total, " << ns_per_qry_our << " ns/op (" << qry_cnt << " ops)\n"
            << "  set.insert: " << us_set_ins << " us total, " << ns_per_ins_set << " ns/op\n"
            << "  set.query : " << us_set_qry << " us total, " << ns_per_qry_set << " ns/op\n";
    }

    return 0;
}
