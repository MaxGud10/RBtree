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
    std::weak_ptr  <Node<KeyT>> parent_; // неплохая статья про цикличность укащатель (они зависят друг от друга. то есть parent зависит от детей и наоборот)
    std::shared_ptr<Node<KeyT>> left_   = nullptr; // https://habr.com/ru/companies/piter/articles/706866/
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

    // weak_ptr -> shared_ptr
    std::shared_ptr<Node<KeyT>> get_parent(const std::shared_ptr<Node<KeyT>>& node) const
    {
        return node ? node->parent_.lock() : nullptr;
    }

    // parent(parent(node)).
    std::shared_ptr<Node<KeyT>> get_grandparent(const std::shared_ptr<Node<KeyT>>& node) const
    {
        auto   parent_node = get_parent(node);
        return parent_node ? parent_node->parent_.lock() : nullptr;
    }

    void fix_insert(std::shared_ptr<Node<KeyT>> current_node)
    {
        while (current_node && current_node != root_)
        {
            auto parent_node      = get_parent(current_node);
            if (!parent_node || parent_node->color != Color::red)
                break;

            auto grandparent_node = get_grandparent(current_node);
            if (!grandparent_node)
                break;

            // Случай: родитель - левый ребёнок дедушки
            if (parent_node == grandparent_node->left_)
            {
                auto uncle_node = grandparent_node->right_;

                // Дядя красный: перекраска без поворотов
                if (uncle_node && uncle_node->color == Color::red)
                {
                    parent_node->color      = Color::black;
                    uncle_node->color       = Color::black;
                    grandparent_node->color = Color::red;
                    current_node            = grandparent_node;
                }
                else
                {
                    // Треугольник: сначала поворот к «линии»
                    if (current_node == parent_node->right_)
                    {
                        current_node = parent_node;
                        left_rotate(current_node);

                        parent_node      = get_parent(current_node);
                        grandparent_node = parent_node ? parent_node->parent_.lock() : nullptr;
                    }

                    if (parent_node && grandparent_node)
                    {
                        parent_node->color      = Color::black;
                        grandparent_node->color = Color::red;
                        right_rotate(grandparent_node);
                    }
                }
            }
            // Случай: родитель - правый ребёнок дедушки
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
                        grandparent_node = parent_node ? parent_node->parent_.lock() : nullptr;
                    }

                    if (parent_node && grandparent_node)
                    {
                        parent_node->color      = Color::black;
                        grandparent_node->color = Color::red;
                        left_rotate(grandparent_node);
                    }
                }
            }
        }

        root_->color = Color::black;
    }

    // балансировка после вставки ключа 
    // void fix_insert(std::shared_ptr<Node<KeyT>> node)
    // {
    //     while (node != root_ && node->parent_->color == Color::red) 
    //     {
    //         if (!node->parent_ || !node->parent_->parent_) 
    //             break; 

    //         if (node->parent_ == node->parent_->parent_->left_) 
    //         {
    //             auto uncle = node->parent_->parent_->right_;
    //             if (uncle && uncle->color == Color::red) 
    //             {
    //                 fix_insert_without_rotate(node, uncle);
    //             }
    //             else 
    //             {
    //                 if (node == node->parent_->right_) 
    //                 {
    //                     node = node->parent_;
    //                     left_rotate(node);
    //                 }

    //                 node->parent_->color           = Color::black;
    //                 node->parent_->parent_->color  = Color::red;

    //                 right_rotate(node->parent_->parent_);
    //                 node = node->parent_;
    //             }
    //         } 
                
    //         else
    //         {
    //             auto uncle = node->parent_->parent_->left_;

    //             if (uncle && uncle->color == Color::red)                
    //                 fix_insert_without_rotate(node, uncle);

    //             else 
    //             {
    //                 if (node == node->parent_->left_) 
    //                 {
    //                     node = node->parent_;
    //                     right_rotate(node);
    //                 }

    //                 node->parent_->color          = Color::black;
    //                 node->parent_->parent_->color = Color::red;
                    
    //                 left_rotate(node->parent_->parent_);
    //                 node = node->parent_;
    //             }
    //         }
    //     }

    //     root_->color = Color::black;
    // }

    void fix_insert_without_rotate(std::shared_ptr<Node<KeyT>> node, 
                                   std::shared_ptr<Node<KeyT>> uncle) 
    {
        node->parent_->color = Color::black;
        if (uncle) 
            uncle->color     = Color::black;

        node->parent_->parent_->color = Color::red;
        node                          = node->parent_->parent_;
    }

void left_rotate(const std::shared_ptr<Node<KeyT>> pivot_node)
    {
        if (!pivot_node)
            return;

        auto new_root = pivot_node->right_;
        if (!new_root)
            return;

        // Поднимаем левое поддерево new_root на место правого поддерева pivot_node.
        pivot_node->right_ = new_root->left_;
        if (new_root->left_)
            new_root->left_->parent_ = pivot_node; // weak_ptr <= shared_ptr

        // Привязать new_root к бывшему родителю pivot_node.
        auto parent_of_pivot = pivot_node->parent_.lock();
        new_root->parent_    = parent_of_pivot;

        if (!parent_of_pivot)
            root_ = new_root;
    
        else if (pivot_node == parent_of_pivot->left_)
            parent_of_pivot->left_ = new_root;
    
        else
            parent_of_pivot->right_ = new_root;
        

        // Сделать pivot_node левым ребёнком new_root.
        new_root->left_     = pivot_node;
        pivot_node->parent_ = new_root;
    }

    // void left_rotate(const std::shared_ptr<Node<KeyT>> node) 
    // {
    //     if (!node)
    //         return;

    //     auto new_root = node->right_;
    //     if (!new_root)
    //         return;

    //     auto y = node->right_;
    //              node->right_ = y->left_;

    //     if (y->left_) 
    //         y->left_->parent_ = node;
    //     y->parent_ = node->parent_;

    //     if (!node->parent_) 
    //         root_ = y;

    //     else if (node == node->parent_->left_) 
    //         node->parent_->left_ = y;

    //     else 
    //         node->parent_->right_ = y;

    //     y->left_ = node;
    //                node->parent_ = y;
    // }
    
    void right_rotate(const std::shared_ptr<Node<KeyT>> pivot_node)
    {
        if (!pivot_node)
            return;

        auto new_root = pivot_node->left_;
        if (!new_root)
            return;

        // Поднимаем правое поддерево new_root на место левого поддерева pivot_node.
        pivot_node->left_ = new_root->right_;
        if (new_root->right_)
            new_root->right_->parent_ = pivot_node;

        // Привязать new_root к бывшему родителю pivot_node.
        auto parent_of_pivot = pivot_node->parent_.lock();
        new_root->parent_    = parent_of_pivot;

        if (!parent_of_pivot)
            root_ = new_root;
        
        else if (pivot_node == parent_of_pivot->right_)
            parent_of_pivot->right_ = new_root;
        
        else
            parent_of_pivot->left_ = new_root;

        // Сделать pivot_node правым ребёнком new_root.
        new_root->right_    = pivot_node;
        pivot_node->parent_ = new_root;
    }

    // void right_rotate(const std::shared_ptr<Node<KeyT>> node)  
    // {
    //     if (!node || !node->left_)  return;

    //     auto y = node->left_;
    //              node->left_ = y->right_;

    //     if (y->right_) 
    //         y->right_->parent_ = node;
    //     y->parent_ = node->parent_;

    //     if (!node->parent_) 
    //         root_ = y;

    //     else if (node == node->parent_->right_) 
    //         node->parent_->right_ = y;

    //     else 
    //         node->parent_->left_ = y;

    //     y->right_ = node;
    //                 node->parent_ = y;
    // }


public:
    Red_black_tree() = default;

    Red_black_tree(KeyT key) 
    {
        root_ = std::make_shared<Node<KeyT>>(key, Color::black);
    }

    std::shared_ptr<const Node<KeyT>> get_root() const { return root_; }

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