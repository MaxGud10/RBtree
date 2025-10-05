#pragma once

#include <iostream>
#include <memory>

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
    Node(KeyT key) : key_(key), color(Color::red) {}

    Node(Node&& rhs) 
    {
        key_  = rhs.key_;
        color = rhs.color;
    }

    Node& operator=(const Node& rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        key_    = rhs.key_;
        color   = rhs.color;
        parent_ = rhs.parent_;
        left_   = rhs,left_;
        right_  = rhs.right_;

        return *this;
    }

    // void operator%(const Node& rhs)
    // {
    //     std::count << "operator %\n";
    // }
};

template <typename KeyT>
class Red_black_tree
{
public:
    std::shared_ptr<Node<KeyT>> root_  = nullptr;

private:
    // балансировка после вставки ключа 
    void fix_insert(std::shared_ptr<Node<KeyT>> node)
    {
        while(node != root_ && node->parent_->color == Color::red)
        {
            if (node->parent_ && node->parent_->parent_)
            {
                if (node->parent_ == node->parent_->parent_->left_)
                {
                    auto uncle = node->parent_->parent_->right_;
                    if (uncle && uncle->color == Color::red)
                    {
                        node->parent_->color = Color::black;
                        uncle->color         = Color::black;

                        node->parent_->parent_->color = Color::red;
                        node = node->parent_->parent_;
                    }

                    else
                    {
                        if (node == node->parent_->right_)
                        {
                            node = node->parent_;
                            left_rotate(node);
                        }

                        node->parent_->color          = Color::black;
                        node->parent_->parent_->color = Color::red;

                        right_rotate(node->parent_->parent_);
                    }
                }
                
                else
                {
                    auto uncle = node->parent_->parent_->left_;

                    if (uncle && uncle->color == Color::red)
                    {
                        node->parent_->color = Color::black;
                        uncle->color         = Color::black;

                        node->parent_->parent_->color = Color::red;
                        node = node->parent_->parent_;
                    }

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
        }

        root_->color = Color::black;
    }


    void left_rotate(std::shared_ptr<Node<KeyT>> node) // TODO проверить есть ли ребенок 
    {
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

    void right_rotate(std::shared_ptr<Node<KeyT>> node)  // TODO проверить есть ли ребенок 
    {
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
    Red_black_tree(KeyT key) 
    {
        root_ = std::make_shared<Node<KeyT>> (key);
        root_->color = Color::black;
    }

    // вставка ключа 
    void insert_elem(KeyT key)
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

    bool search(KeyT key1, KeyT key2) const
    {

        uint64_t counter = 0;
        //std::shared_ptr<Node<KeyT>> node = root_;

        search_(root_, counter, key1, key2);

        // while (node) { // для key1

        //     if (key1 < *node) {
        //         node = node->right_
        //         counter++;
        //     }
        // }
        std::cout << "counter = " << counter << std::endl;

        return true;
    }


    void search_(std::shared_ptr<Node<KeyT>> node, uint64_t& counter, KeyT key1, KeyT key2) const 
    {
        if (node->key_ >= key1 && node->key_ <= key2)
        {
            counter++;
            if (node->left_)
                search_(node->left_, counter, key1, key2);

            if (node->right_)
            search_(node->right_, counter, key1, key2);
        }

        else if (node->key_ < key1 && node->right_) 
        {
            search_(node->right_, counter, key1, key2);
        }

        else if (node->key_ > key2 && node->left_) 
        {
            search_(node->left_, counter, key1, key2);
        }

        return;
    }

    void delete_node() 
    {

    }

    void print(Node<KeyT>& node, std::ofstream& file_name) 
    {
       // std::cout << " { " << node.key_ << " color: " << static_cast<int>(node.color);
        if (node.left_)
        {
            file_name 
            << node.key_ << " [shape = Mrecord, style = filled, fillcolor = " << (static_cast<int>(node.color) == 0 ? "maroon" : "Gray") << ", label = \"" <<  node.key_ << "\" ];\n"
            << node.left_->key_ << " [shape = Mrecord, style = filled, fillcolor = " << (static_cast<int>(node.left_->color) == 0 ? "maroon" : "Gray") << ", label = \"" << node.left_->key_ << "\" ];\n"
            << node.key_ << " -> " << node.left_->key_ << ";\n";
            print(*node.left_, file_name);
        }
        if (node.right_) 
        {
            file_name 
            << node.key_ << " [shape = Mrecord, style = filled, fillcolor = " << (static_cast<int>(node.color) == 0 ? "maroon" : "Gray") << ", label = \"" <<  node.key_ << "\" ];\n"
            << node.right_->key_ << " [shape = Mrecord, style = filled, fillcolor = " << (static_cast<int>(node.right_->color) == 0 ? "maroon" : "Gray") << ", label = \"" << node.right_->key_ << "\" ];\n"
            << node.key_ << " -> " << node.right_->key_ << ";\n";
            print(*node.right_, file_name);
        }
        //std::cout << " }";
    }
};


template <typename KeyT>
class binary_tree 
{
    Node<KeyT> root_;
    void print(Node<KeyT>& node) 
    {
        std::cout << " { " << node.key_;

        if (node.left_) 
        {
            print(*node.left_);
        }

        if (node.right_) 
        {
            print(*node.right_);
        }

        std::cout << " }";
    }
};

};