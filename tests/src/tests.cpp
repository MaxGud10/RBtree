#include <iostream>
#include <fstream>
#include <cstdint>
#include <chrono>
#include <set>
#include <string>

#include "red_black_tree.hpp"

static int  run_tests();
static int  run_big_test();
static void range_queries_set(const std::string& file_name);

int main() 
{
    return run_tests() + run_big_test();
}

int run_tests() 
{
    const int TEST_NUM = 9;

    std::ifstream test_file;
    test_file.open("./tests/tests.txt");
    if (!test_file.is_open())
        return -1;

    int64_t a = 0; 
    int64_t b = 0;

    int64_t key; 
    int64_t counter_ref;

    char mode;
    bool all_tests_pass = true;
    int  test_counter   = 0;
 
    for (int i = 0; i != TEST_NUM; ++i)  
    {

        test_file >> mode;
        if (mode != 'k')
            break;

        test_file >> key;
        Tree::Red_black_tree<int64_t> rb_tree{key};

        while(test_file >> mode)
        {
            if (mode == 'k')
            {
                test_file >> key;
                rb_tree.insert_elem(key);
            }

            else if (mode == 'q') 
            {
                test_file >> a >> b;
                    
                int64_t counter;
                if (a > b) 
                    counter = 0;

                else 
                   counter = rb_tree.range_queries(a, b);

                test_file >> counter_ref;
                if (counter != counter_ref) 
                {
                    std::cout << "test [" << test_counter << "] failed \n";
                    all_tests_pass = false;
                }

                ++test_counter;
                break;
            }
            else 
                break;
        }
    }

    if (all_tests_pass) 
    {
        std::cout << "-- All tests passed -- \n";
        return 0;
    }

    else 
        return -1;
}

int run_big_test() 
{
    std::ifstream test_file;
    test_file.open("./tests/range_query_test.txt");
    if (!test_file.is_open())
        return -1;
        
    const int64_t ref_array[] = {606, 56, 727, 724, 135, 752, 468, 158, 650, 743, 208, 406, 514, 627, 493, 883, 91, 199, 278, 265, 
        60, 203, 534, 374, 249, 563, 736, 366, 20, 345, 223, 224, 513, 640, 539, 614, 471, 200, 195, 467, 113, 541, 305, 779, 69, 
        93, 102, 268, 231, 437, 383, 1, 246, 96, 133, 260, 32, 130, 763, 586, 341, 371, 80, 574, 633, 322, 126, 107, 179, 326, 313, 
        7, 168, 245, 841, 111, 457, 463, 80, 764, 37, 304, 462, 528, 599, 208, 256, 803, 52, 63, 31, 688, 889, 139, 294, 309, 449, 
        155, 52, 258, 737, 51, 55, 205, 325, 425, 881, 894, 328, 133, 111, 489, 361, 140, 346, 513, 276, 374, 491, 8, 828, 691, 142,
        403, 394, 280, 183, 341, 388, 442, 457, 225, 123, 674, 554, 544, 403, 526, 722, 195, 590, 39, 773, 947, 79, 716, 875, 100, 14, 
        341, 220, 384, 320, 77, 281, 424, 460, 20, 194, 543, 225, 132, 7, 258, 160, 516, 371, 713, 392, 20, 249, 232, 917, 201, 336, 551, 
        336, 24, 177, 153, 370, 548, 352, 549, 93, 649, 43, 327, 237, 754, 56, 299, 388, 311, 315, 239, 193, 128, 387, 263, 658, 398, 668, 
        661, 284, 710, 656, 525, 91, 383, 140, 106, 926, 356, 342, 174, 373, 139, 498, 651, 270, 293, 179, 381, 574, 324, 93, 34, 683, 7, 
        69, 646, 367, 32, 265, 44, 730, 154, 179, 542, 333, 454, 521, 219, 611, 254, 647, 318, 423, 526, 534, 23, 657, 377, 244, 276, 50, 
        447, 170, 246, 340, 591, 387, 42, 88, 128, 116, 568, 41, 503, 283, 778, 356, 214, 644, 378, 89, 725, 206, 425, 77, 343, 388, 186, 
        33, 168, 680, 514, 417, 384, 441, 389, 789, 287, 327, 45, 792, 262, 600, 190, 111, 506, 214, 433, 141, 81, 107, 877, 266, 108, 492, 
        308, 96, 24, 462, 232, 76, 613, 936, 46, 483, 178, 132, 349, 66, 384, 492, 18, 490, 284, 273, 254, 160, 179, 390, 618, 36, 507, 310,
        366, 177, 67, 674, 35, 816, 762, 150, 551, 190, 102, 381, 635, 73, 200, 289, 331, 856, 513, 930, 197, 28, 104, 34, 287, 323, 423, 289, 
        286, 710, 154, 134, 68, 514, 443, 467, 430, 299, 159, 658, 8, 158, 72, 52, 444, 235, 78, 772, 349, 582, 717, 282, 9, 43, 446, 507, 422, 
        494, 596, 208, 891, 145, 88, 71, 626, 63, 222, 279, 38, 276, 222, 59, 327, 623, 162, 456, 41, 520, 653, 157, 73, 132, 69, 73, 904, 165, 
        261, 443, 593, 669, 681, 116, 389, 616, 250, 278, 149, 401, 366, 55, 27, 410, 220, 753, 726, 130, 512, 345, 501, 37, 255, 67, 661, 4, 213, 
        109, 237, 344, 149, 134, 937, 829, 358, 74, 131, 454, 189, 785, 292, 12, 66, 249, 800, 55, 712, 471, 430, 18, 592, 639, 510, 336, 161, 
        814, 255, 44, 237, 54, 526, 563, 339, 51, 706, 332, 164, 55, 225, 487, 458, 389, 89};    

    int64_t a = 0;
    int64_t b = 0;

    int64_t key;
    int64_t test_counter   = 0;
    bool    all_tests_pass = true;

    #ifdef SET_MODE_ENABLED
        auto start_rb = std::chrono::high_resolution_clock::now();
    #endif

    char mode;
    test_file >> mode;
    if (mode != 'k')
        return -1;

    test_file >> key;
    Tree::Red_black_tree<int64_t> rb_tree{key};

    while (!test_file.eof())
    {
        while((test_file >> mode).good()) 
        {
            if (mode == 'k') 
            {
                test_file >> key;
                rb_tree.insert_elem(key);
            }

            else if (mode == 'q') 
            {
                test_file >> a >> b;
                    
                int64_t counter;
                if (a > b) 
                    counter = 0;
                else 
                   counter = rb_tree.range_queries(a, b);

                if (counter != ref_array[test_counter])
                {
                    std::cout << "test [" << test_counter << "] failed \n";
                    all_tests_pass = false;
                }

                ++test_counter;
                break;
            }
            else 
                break;
        }
    }
    test_file.close();

    #ifdef SET_MODE_ENABLED
        auto end_rb = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_rb = end_rb - start_rb;
        std::cout << "Program rb_tree execution time: " << elapsed_rb.count() << " seconds\n";
   
        auto start_set = std::chrono::high_resolution_clock::now();
        range_queries_set("./tests/range_query_test.txt");
        auto end_set = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_set = end_set - start_set;
        std::cout << "Program set execution time:     " << elapsed_set.count() << " seconds\n";
    #endif

    if (all_tests_pass) 
    {
        std::cout << "-- All big tests passed -- \n";
        return 0;
    }

    else 
        return -1;

}
void range_queries_set(const std::string& file_name) 
{
    std::set<int64_t> set;

    std::ifstream test_file;
    test_file.open(file_name);
    if (!test_file.is_open())
        return;

    char mode;
    int64_t a, b, key;
    while (!test_file.eof()) 
    {

        if (mode == 'k') 
        {
            test_file >> key;
            set.insert(key);
        }

        else if (mode == 'q') 
        {
            test_file >> a >> b;
                
            int64_t counter;
            if (a > b) 
                counter = 0;

            else 
            {
                auto l1 = set.lower_bound(a);
                auto l2 = set.upper_bound(b);

                if (std::distance(set.begin(), l1) > std::distance(set.begin(), l2))
                    std::swap(l1, l2);
                counter = std::distance(l1, l2);
            }
        }
        
        else 
            break;
    }
    test_file.close();
}