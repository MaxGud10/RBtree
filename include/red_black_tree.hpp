#pragma once

#include <iostream>
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
    Node* parent_ = nullptr;     
    Node* left_   = nullptr;     
    Node* right_  = nullptr;  

    Color color;
    KeyT  key_;

    Node() = default;

    Node(const KeyT &key, Color color = Color::red) : key_{key}, color{color} {}
};

template <typename KeyT>
class Red_black_tree
{
    Node<KeyT>* root_  = nullptr;

    Node<KeyT>* get_parent(Node<KeyT>* node) const
    {
        return node ? node->parent_ : nullptr;
    }

    Node<KeyT>* get_grandparent(Node<KeyT>* node) const 
    {
        Node<KeyT>* parent_node = get_parent(node);

        return parent_node ? parent_node->parent_ : nullptr;
    }

    // балансировка после вставки ключа
    void fix_insert(Node<KeyT>* current_node)
    {
        while (current_node && current_node != root_)
        {
            auto parent_node      = get_parent(current_node);
            if (!parent_node || parent_node->color != Color::red)
                break;

            auto grandparent_node = get_grandparent(current_node);
            if (!grandparent_node)
                break;

            // родитель - левый ребёнок дедушки
            if (parent_node == grandparent_node->left_)
            {
                auto uncle_node = grandparent_node->right_;

                // if дядя красный => перекраска без поворотов
                if (uncle_node && uncle_node->color == Color::red)
                {
                    parent_node->color      = Color::black;
                    uncle_node->color       = Color::black;
                    grandparent_node->color = Color::red;
                    current_node            = grandparent_node;
                }
                else
                {
                    if (current_node == parent_node->right_)
                    {
                        current_node = parent_node;
                        left_rotate(current_node);

                        parent_node      = get_parent(current_node);
                        grandparent_node = get_grandparent(current_node);
                        if (!parent_node || !grandparent_node) break;
                    }

                    parent_node->color      = Color::black;
                    grandparent_node->color = Color::red;
                    right_rotate(grandparent_node);
                }
            }
            // родитель - правый ребёнок дедушки
            else
            {
                auto uncle_node = grandparent_node->left_;

                if (uncle_node && uncle_node->color == Color::red)
                {
                    parent_node->color      = Color::black;
                    uncle_node->color       = Color::black;
                    grandparent_node->color = Color::red;
                    current_node            = grandparent_node;
                }
                else
                {
                    if (current_node == parent_node->left_)
                    {
                        current_node = parent_node;
                        right_rotate(current_node);

                        parent_node      = get_parent(current_node);
                        grandparent_node = get_grandparent(current_node);
                        if (!parent_node || !grandparent_node) break;
                    }

                    parent_node->color      = Color::black;
                    grandparent_node->color = Color::red;
                    left_rotate(grandparent_node);
                }
            }
        }

        root_->color = Color::black;
    }

    void left_rotate(Node<KeyT>* pivot_node)
    {
        if (!pivot_node) 
            return;

        Node<KeyT>* new_root = pivot_node->right_;

        if (!new_root) 
            return;

        pivot_node->right_ = new_root->left_;
        if (new_root->left_)
            new_root->left_->parent_ = pivot_node;

        Node<KeyT>* parent_of_pivot = pivot_node->parent_;
        new_root->parent_           = parent_of_pivot;

        if (!parent_of_pivot)
            root_ = new_root;

        else if (pivot_node == parent_of_pivot->left_)
            parent_of_pivot->left_ = new_root;

        else
            parent_of_pivot->right_ = new_root;

        new_root->left_ = pivot_node;
                          pivot_node->parent_ = new_root;
    }

    void right_rotate(Node<KeyT>* pivot_node)
    {
        if (!pivot_node) 
            return;

        Node<KeyT>* new_root = pivot_node->left_;

        if (!new_root) 
            return;

        pivot_node->left_ = new_root->right_;
        if (new_root->right_)
            new_root->right_->parent_ = pivot_node;

        Node<KeyT>* parent_of_pivot = pivot_node->parent_;
        new_root->parent_           = parent_of_pivot;

        if (!parent_of_pivot)
            root_ = new_root;

        else if (pivot_node == parent_of_pivot->right_)
            parent_of_pivot->right_ = new_root;

        else
            parent_of_pivot->left_ = new_root;

        new_root->right_ = pivot_node;
                           pivot_node->parent_ = new_root;
    }

    static void destroy_subtree(Node<KeyT>* node) noexcept
    {
        if (!node) 
            return;
        destroy_subtree(node->left_);
        destroy_subtree(node->right_);

        delete node;
    }

// TODO: может быть потом написать свой метод distanse 

public:
    Red_black_tree() = default;

    Red_black_tree(KeyT key) 
    {
        root_ = new Node<KeyT> (key, Color::black);
    }

    ~Red_black_tree()
    {
        destroy_subtree(root_);
        root_ = nullptr;
    }

    Red_black_tree(const Red_black_tree&)            = delete;
    Red_black_tree& operator=(const Red_black_tree&) = delete;


    Red_black_tree(Red_black_tree&& other) : root_(other.root_)
    {
        other.root_ = nullptr;
    }

    Red_black_tree& operator=(Red_black_tree&& other) 
    {
        if (this != &other)
        {
            destroy_subtree(root_);

            root_ = other.root_;
                    other.root_ = nullptr;
        }

        return *this;
    }

    const Node<KeyT>* get_root() const { return root_; }


    // вставка ключа
    void insert_elem(const KeyT key)
    {
        Node<KeyT>* parent  = nullptr;
        Node<KeyT>* current = root_;

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

        // cоздаем нлвый узел и привязать
        Node<KeyT>* new_node          = new Node<KeyT>(key, Color::red);
                    new_node->parent_ = parent;

        if (!parent)
        {
            root_        = new_node;     // первый узел
            root_->color = Color::black; // корень всегда чёрный

            return;
        }

        else if (key < parent->key_)
        {
            parent->left_ = new_node;
        }

        else
        {
            parent->right_ = new_node;
        }

        fix_insert(new_node); // балансировка 
    }

    uint64_t range_queries(const KeyT key1, const KeyT key2) const 
    { 
        uint64_t counter = 0;

        search(root_, counter, key1, key2);

        return counter;
    }


private:
    void search(const Node<KeyT>* node, uint64_t& counter, const KeyT& key1, const KeyT& key2) const
    {
        if (!node) 
            return;

        if (node->key_ >= key1 && node->key_ <= key2)
        {
            ++counter;

            if (node->left_ ) 
                search(node->left_,  counter, key1, key2);

            if (node->right_) 
                search(node->right_, counter, key1, key2);
        }

        else if (node->key_ < key1)
        {
            if (node->right_) 
                search(node->right_, counter, key1, key2);
        }

        else // node->key_ > key2
        {
            if (node->left_)  
                search(node->left_,  counter, key1, key2);
        }
    }
};

}; // namespace Tree



namespace RangeQueries 
{

template<typename KeyT>
class Range_queries 
{
public:
    Tree::Red_black_tree<KeyT> rb_tree;

    void add_element(std::istream& in) 
    {
        KeyT key;
        
        if (!(in >> key))
        {
            std::cerr << "WARN: failed to read key from stdin\n";

            return;
        }

        rb_tree.insert_elem(key);
    }


    int64_t find_range_elements(KeyT a, KeyT b) const 
    {
        if (b <= a) 
        {
            return 0;
        }
        return static_cast<int64_t>(rb_tree.range_queries(a, b));
    }

    int64_t find_range_elements(std::istream& in) 
    {
        KeyT a{}, b{};

        if (!(in >> a >> b)) 
        {
            std::cerr << "WARN: failed to read range from stdin\n";

            return 0;
        }

        return find_range_elements(a, b); 
    }
};

} // namespace RangeQueries