#pragma once

#include <iostream>
#include <memory>
#include <fstream>   
#include <sstream>  
#include <cstdlib>  

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
        left_   = rhs.left_;
        right_  = rhs.right_;

        return *this;
    }
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
    Red_black_tree() = default;

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

    uint64_t range_queries(KeyT key1, KeyT key2) const 
    { 
        uint64_t counter = 0;

        search(root_, counter, key1, key2);

        return counter;
    }


private:
    void search(std::shared_ptr<Node<KeyT>> node, uint64_t& counter, KeyT key1, KeyT key2) const 
    {
        if (node->key_ >= key1 && node->key_ <= key2)
        {
            counter++;
            if (node->left_)
                search(node->left_, counter, key1, key2);

            if (node->right_)
            search(node->right_, counter, key1, key2);
        }

        else if (node->key_ < key1 && node->right_) 
        {
            search(node->right_, counter, key1, key2);
        }

        else if (node->key_ > key2 && node->left_) 
        {
            search(node->left_, counter, key1, key2);
        }

        return;
    }

    std::shared_ptr<Node<KeyT>> lower_bound(KeyT key) const;
    std::shared_ptr<Node<KeyT>> upper_bound(KeyT key) const; 

    void delete_node() 
    {

    }

    mutable std::size_t nil_counter_ = 0; // уникальные имена для NULL-листьев

    void emit_node_(const Node<KeyT>& node, std::ofstream& out, bool is_root) const
    {
        const char* fill = is_root ? "#5A5A5A"                  
                                   : (node.color == Color::red ? "#D85C5C"  
                                   : "#BDBDBD"); 

        const char* fontcolor = is_root ? "white" : "black";


        std::ostringstream parent_ss, left_ss, right_ss;
        if (auto p = node.parent_) parent_ss << p->key_; else parent_ss << "NIL";
        if (node.left_ ) left_ss  << node.left_->key_;   else left_ss   << "NIL";
        if (node.right_) right_ss << node.right_->key_;  else right_ss  << "NIL";

        out << node.key_
            << " [shape=Mrecord, style=filled, fillcolor=\"" << fill
            << "\", fontcolor=\"" << fontcolor
            << "\", label=\"{ key: " << node.key_
            << " | parent: " << parent_ss.str()
            << " | { L: " << left_ss.str() << " | R: " << right_ss.str()
            << " } }\" ];\n";
    }

public:
    void print(const Node<KeyT>& node, std::ofstream& out, bool is_root = false) const
    {
        emit_node_(node, out, is_root);

        if (node.left_) 
        {
            out << node.key_ << " -> " << node.left_->key_ << ";\n";
            print(*node.left_, out, /*is_root=*/false);
        } 

        else 
        {
            std::string nil_id = "nilL_" + std::to_string(nil_counter_++);
            out << nil_id
                << " [shape=box, style=filled, fillcolor=\"#BDBDBD\", label=\"NULL\"];\n";
            out << node.key_ << " -> " << nil_id << ";\n";
        }

        if (node.right_) 
        {
            out << node.key_ << " -> " << node.right_->key_ << ";\n";
            print(*node.right_, out, /*is_root=*/false);
        } 

        else 
        {
            std::string nil_id = "nilR_" + std::to_string(nil_counter_++);
            out << nil_id
                << " [shape=box, style=filled, fillcolor=\"#BDBDBD\", label=\"NULL\"];\n";
            out << node.key_ << " -> " << nil_id << ";\n";
        }
    }


    void dump(const std::string& dot_path = "file_graph.dot",
              const std::string& png_path = "tree_graph.png",
              bool auto_open = true) const
    {
        std::ofstream file(dot_path);
        if (!file)
            return; 


        file << "digraph RB_tree {\n"
                "label = < Red-black tree >;\n"
                "bgcolor = \"#BAF0EC\";\n"
                "rankdir=TB;\n"
                "node  [shape=record, style=filled];\n"
                "edge  [color=black, arrowsize=0.8];\n";

        if (root_)
            print(*root_, file, /*is_root=*/true);

        file << "}\n";
        file.close();

        // генерируем png
        {
            std::string cmd = "dot -Tpng \"" + dot_path + "\" -o \"" + png_path + "\"";
            (void)std::system(cmd.c_str());
        }

    }
};

};