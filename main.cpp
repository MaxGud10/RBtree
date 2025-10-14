#include <iostream>
#include <sstream>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <string>
#include <set>

#include "red_black_tree.hpp"
#include "graphic_dump.hpp"


static int64_t range_queries_set(const std::set<int64_t>& set, int64_t a, int64_t b);

int main() 
{
    #ifdef SET_MODE_ENABLED
        std::set<int64_t> set; 
    #endif

    uint64_t key = 0; 
    char     mode;

    RangeQueries::Range_queries<int64_t> range_quer;

    int64_t a = 0;
    int64_t b = 0;

    // while((std::cin >> mode).good()) 
    // {
    //     switch (mode) 
    //     {
    //         case 'k': 
    //         {
    //             if (!(std::cin >> key))
    //             {
    //                 std::cerr << "ERROR: expected key after 'k'\n";
    //                 return 1;
    //             }

    //             range_quer.rb_tree.insert_elem(key);

    //             #ifdef SET_MODE_ENABLED
    //                 set.insert(key);
    //             #endif

    //             break;
    //         }

    //         case 'q': 
    //         {
    //             if (!(std::cin >> a >> b))
    //             {
    //                 std::cerr << "ERROR: expected two numbers after 'q'\n";
    //                 return 1;
    //             }

    //             if (a > b)
    //                 std::swap(a, b);

    //             auto ans = range_quer.find_range_elements(a, b);
    //             std::cout << ans << ' ';

    //             #ifdef SET_MODE_ENABLED
    //                 auto ans_set = range_queries_set(set, a, b);
    //                 std::cerr << "DBG: set range [" << a << ", " << b << "] -> " << ans_set << "\n";
    //             #endif

    //             break;
    //         }

    //         default:
    //             std::cerr << "ERROR: unknown mode: '" << mode << "'\n";
    //             return 1;
    //     }
    // }

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

                range_quer.rb_tree.insert_elem(key);

                #ifdef SET_MODE_ENABLED
                    set.insert(key);
                #endif

                break;
            }

            case 'q': 
            {
                if (!(std::cin >> a >> b)) 
                {
                    std::cerr << "ERROR: expected two numbers after 'q'\n";
                    return 1;
                }

                const auto ans = range_quer.find_range_elements(a, b);
                std::cout << ans << ' ';

                #ifdef SET_MODE_ENABLED
                    const auto check = range_queries_set(set, a, b);
                    if (check != ans) {
                        std::cerr << "DBG set=" << check
                                  << " tree="   << ans
                                  << " for q "  << a   << ' ' << b << '\n';
                    }
                #endif

                break;
            }

            default:
                std::cerr << "ERROR: unknown mode: '" << mode << "'\n";
                return 1;
        }
    }

    std::cout << std::endl;

    #ifdef CUSTOM_MODE_DEBUG
        Tree::Print_tree<int64_t> pr_tr;
        pr_tr.dump(range_quer.rb_tree, "graphviz/file_graph.dot", "graphviz/tree_graph.png", true);
    #endif

    return 0;
}

int64_t range_queries_set(const std::set<int64_t>& set, int64_t a, int64_t b) 
{
    if (b <= a) 
        return 0;

    auto first = set.lower_bound(a);
    auto last  = set.upper_bound(b);

    return std::distance(first, last);
}