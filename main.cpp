#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <set>

#include "red_black_tree.hpp"


static int64_t range_queries_set(const std::set<int64_t>& set, int64_t a, int64_t b);

int main() 
{
    #ifdef SET_MODE_ENABLED
        std::set<int64_t> set; 
    #endif

    uint64_t key = 0; 
    char     mode;

    Tree::Red_black_tree<uint64_t> rb_tree;

    int64_t a = 0;
    int64_t b = 0;

    while((std::cin >> mode).good()) 
    {
        if (mode == 'k') 
        {
            if (!(std::cin >> key).good())
                break;

            #ifdef SET_MODE_ENABLED
                set.insert(key); 
            #endif
            rb_tree.insert_elem(key);
        }

        else if (mode == 'q') 
        {
            if (!(std::cin >> a >> b).good())
                continue;
            if (a > b)  
            {
                std::swap(a, b);

                #ifdef SET_MODE_ENABLED
                    std::cout << "set = " << "0 " << ' ';
                #endif 

                continue;
            }

            int64_t counter = rb_tree.range_queries(a, b);
            std::cout << counter << ' ';

            #ifdef SET_MODE_ENABLED
                int64_t counter_set = range_queries_set(set, a, b);
                std::cout << "set = " << counter_set << ' ';
            #endif 
        }

        else 
            break;
    }

    std::cout << std::endl;

    #ifndef NDEBUG
        rb_tree.dump();
    #endif

    return 0;
}

int64_t range_queries_set(const std::set<int64_t>& set, int64_t a, int64_t b) 
{
    auto l1 = set.lower_bound(a);
    auto l2 = set.upper_bound(b);
    if (std::distance(set.begin(), l1) > std::distance(set.begin(), l2))
        std::swap(l1, l2);

    return std::distance(l1, l2);
}