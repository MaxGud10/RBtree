#pragma once

#include <iostream>
#include <memory>
#include <fstream>   
#include <sstream>  
#include <cstdlib>  
#include <filesystem>

namespace Tree
{

enum class Color
{
    red,
    black,
};     

template <typename KeyT>
struct Node 
{
    // shared_ptr - смарт указатель, который позволяет нескольким shared_ptr совместо владеть один и тем же объектом
    // то есть у нас может быть несколько указателей shared_ptr на один и тот же объект по этому адресу    
    std::shared_ptr<Node<KeyT>> parent_ = nullptr;
    std::shared_ptr<Node<KeyT>> left_   = nullptr;
    std::shared_ptr<Node<KeyT>> right_  = nullptr;

    Color color;
    KeyT  key_;

    Node() = default;

    Node(const KeyT &key, Color color = Color::red) : key_{key}, color{color} {}
};

template <typename KeyT>
class Red_black_tree
{
    std::shared_ptr<Node<KeyT>> root_  = nullptr;

    // балансировка после вставки ключа 
    void fix_insert(std::shared_ptr<Node<KeyT>> node)
    {
        while (node != root_ && node->parent_->color == Color::red) 
        {
            if (!node->parent_ || !node->parent_->parent_) 
                break; 

            if (node->parent_ == node->parent_->parent_->left_) 
            {
                auto uncle = node->parent_->parent_->right_;
                if (uncle && uncle->color == Color::red) 
                {
                    fix_insert_without_rotate(node, uncle);
                }
                else 
                {
                    if (node == node->parent_->right_) 
                    {
                        node = node->parent_;
                        left_rotate(node);
                    }

                    node->parent_->color           = Color::black;
                    node->parent_->parent_->color = Color::red;

                    right_rotate(node->parent_->parent_);
                }
            } 
                
            else
            {
                auto uncle = node->parent_->parent_->left_;

                if (uncle && uncle->color == Color::red)                
                    fix_insert_without_rotate(node, uncle);

                else 
                {
                    if (node == node->parent_->left_) 
                    {
                        node = node->parent_;
                        right_rotate(node);
                    }

                    node->parent_->color          = Color::black;
                    node->parent_->parent_->color = Color::red;
                    
                    left_rotate(node->parent_->parent_);
                }
            }
        }

        root_->color = Color::black;
    }

    void fix_insert_without_rotate(std::shared_ptr<Node<KeyT>> node, 
                                   std::shared_ptr<Node<KeyT>> uncle) 
    {
        node->parent_->color = Color::black;
        uncle->color         = Color::black;

        node->parent_->parent_->color = Color::red;
        node                          = node->parent_->parent_;
    }


    void left_rotate(const std::shared_ptr<Node<KeyT>> node) 
    {
        if (!node || !node->right_) return;

        auto y = node->right_;
                 node->right_ = y->left_;

        if (y->left_) 
            y->left_->parent_ = node;
        y->parent_ = node->parent_;

        if (!node->parent_) 
            root_ = y;

        else if (node == node->parent_->left_) 
            node->parent_->left_ = y;

        else 
            node->parent_->right_ = y;

        y->left_ = node;
                   node->parent_ = y;
    }

    void right_rotate(const std::shared_ptr<Node<KeyT>> node)  
    {
        if (!node || !node->left_)  return;

        auto y = node->left_;
                 node->left_ = y->right_;

        if (y->right_) 
            y->right_->parent_ = node;
        y->parent_ = node->parent_;

        if (!node->parent_) 
            root_ = y;

        else if (node == node->parent_->right_) 
            node->parent_->right_ = y;

        else 
            node->parent_->left_ = y;

        y->right_ = node;
                    node->parent_ = y;
    }


public:
    Red_black_tree() = default;

    Red_black_tree(KeyT key) 
    {
        root_ = std::make_shared<Node<KeyT>>(key, Color::black);
    }

    std::shared_ptr<Node<KeyT>> get_root() const { return root_; }

    // вставка ключа 
    void insert_elem(const KeyT key)
    {
        auto new_node = std::make_shared<Node<KeyT>> (key);
        auto current  = root_;
        std::shared_ptr<Node<KeyT>> parent = nullptr;

        while (current) 
        {
            parent = current;
            if (key < current->key_) 
                current = current->left_;

            else if (key > current->key_)
                current = current->right_;

            else 
                return;
        }

        new_node->parent_ = parent;
        if (!parent) 
            root_ = new_node;

        else if (key < parent->key_) 
            parent->left_ = new_node;

        else 
            parent->right_ = new_node;

        new_node->color = Color::red;
        fix_insert(new_node);         // балансировка    
    }

    uint64_t range_queries(const KeyT key1, const KeyT key2) const 
    { 
        uint64_t counter = 0;

        search(root_, counter, key1, key2);

        return counter;
    }


private:
    void search(const std::shared_ptr<Node<KeyT>> node, uint64_t& counter, const KeyT& key1, const KeyT& key2) const 
    {
        if (!node) return;
        
        if (node->key_ >= key1 && node->key_ <= key2) 
        {
            counter++;
            if (node->left_)
                search(node->left_, counter, key1, key2);

            if (node->right_)
                search(node->right_, counter, key1, key2);
        }

        else if (node->key_ < key1 && node->right_) 
            search(node->right_, counter, key1, key2);
        
        else if (node->key_ > key2 && node->left_) 
            search(node->left_, counter, key1, key2);
        
        return;
    }
};

}; // namespace Tree



namespace RangeQueries 
{

enum class Mode 
{
    cin,
    file
};

template<typename KeyT>
class Range_queries 
{
public:
    Tree::Red_black_tree<KeyT> rb_tree;


    void add_element(Mode mode) 
    {
        if (mode != Mode::cin)
            return;

        KeyT key;
        
        if (!(std::cin >> key))
        {
            std::cerr << "WARN: failed to read key from stdin\n";

            return;
        }

        rb_tree.insert_elem(key);
    }

    void add_element(Mode mode, std::ifstream& file) 
    {
        if (mode != Mode::file)
            return;

        KeyT key;

        if (!(file >> key)) 
        { 
            std::cerr << "[DBG] read key FAIL\n"; return; 
        }

        std::cerr << "[DBG] insert key: " << key << "\n";

        rb_tree.insert_elem(key);
    }


    int64_t find_range_elements(KeyT a, KeyT b) const 
    {
        if (b < a) 
        {
            return 0;
        }
        return static_cast<int64_t>(rb_tree.range_queries(a, b));
    }


    int64_t find_range_elements(Mode mode) 
    {
        if (mode != Mode::cin)
            return 0;

        KeyT a{}, b{};

        if (!(std::cin >> a >> b)) 
        {
            std::cerr << "WARN: failed to read range from stdin\n";
            
            return 0;
        }

        return find_range_elements(a, b); 
    }

    int64_t find_range_elements(Mode mode, std::ifstream& file) 
    {
        if (mode != Mode::file)
            return 0;

        KeyT a{}, b{};

        if (!(file >> a >> b)) 
        {
            std::cerr << "WARN: failed to read range from file\n";

            return 0;
        }

        return ind_range_elements(a,b);; 
    }
};

} // namespace RangeQueries