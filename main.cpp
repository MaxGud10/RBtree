#include <iostream>
#include <cstdint>
#include <memory>

#include "red_black_tree.hpp"

int main() 
{
    std::cout << "start\n";
    uint64_t key = 100;
    int a = 0, b = 0;

    Tree::Red_black_tree<uint64_t> rb_tree{key};

    std::cout << "root was created\n";

    char mode;

    while(std::cin >> mode) 
    {
        if (mode == 'k') 
        {
            std::cout << "insert\n"; 
            std::cin >> key;
            rb_tree.insert_elem(key);
        }

        else if (mode == 'q') 
        {
            std::cin >> a >> b;
            rb_tree.serch(a, b);
        }

        else 
            break;
    }

    rb_tree.print(*rb_tree.root_);

    return 0;
}