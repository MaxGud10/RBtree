#pragma once

#include <cstdlib>
#include <utility>
#include <iterator>
#include <memory>
#include <vector>


#include "rb_iterator.hpp"


namespace Tree
{

enum class Color
{
    red,
    black,
};

namespace detail
{

template <typename KeyT>
struct Node
{
    KeyT  key_;
    Color color;

    Node *parent_ = nullptr;
    Node *left_   = nullptr;
    Node *right_  = nullptr;

    unsigned left_is_thread  : 1;
    unsigned right_is_thread : 1;

    Node() = default;

    Node(const KeyT &key, Color color = Color::red)
        : key_   {key},
          color  {color},
          parent_{nullptr},
          left_  {nullptr},
          right_ {nullptr},
          left_is_thread{1},
          right_is_thread{1} {}
};
} // namespace detail

template <typename KeyT>
class Red_black_tree
{
    using NodeT  = detail::Node<KeyT>;

    NodeT *header_ = nullptr;
    NodeT *root_   = nullptr;

    NodeT *get_parent(NodeT *node) const
    {
        return node ? node->parent_ : nullptr;
    }

    NodeT *get_grandparent(NodeT *node) const
    {
        NodeT *parent_node = get_parent(node);

        return parent_node ? parent_node->parent_ : nullptr;
    }

    NodeT *left_child (NodeT *node) { return (node && !node->left_is_thread)  ? node->left_  : nullptr; }
    NodeT *right_child(NodeT *node) { return (node && !node->right_is_thread) ? node->right_ : nullptr; }

    const NodeT *left_child (const NodeT *node) { return (node && !node->left_is_thread)  ? node->left_  : nullptr; }
    const NodeT *right_child(const NodeT *node) { return (node && !node->right_is_thread) ? node->right_ : nullptr; }

    // balance after insert
    void fix_insert(NodeT *node) noexcept
    {
        while (node && node != root_)
        {
            NodeT *parent = get_parent(node);
            if (!parent || parent == header_ || parent->color != Color::red)
                break;

            NodeT *grand = get_grandparent(node);
            if (!grand || grand == header_)
                break;

            if (parent == left_child(grand))
                node = fix_insert_parent_is_left_(node, parent, grand);
            else
                node = fix_insert_parent_is_right_(node, parent, grand);
        }

        if (root_)
            root_->color = Color::black;
    }

    void recolor_parent_uncle_grand_(NodeT *parent, NodeT *uncle, NodeT *grand) noexcept
    {
        parent->color = Color::black;
        uncle ->color = Color::black;
        grand ->color = Color::red;
    }

    // parent is left child of grandparent
    NodeT *fix_insert_parent_is_left_(NodeT *node, NodeT *parent, NodeT *grand) noexcept
    {
        NodeT *uncle = right_child(grand);

        // uncle is red => recolor and continue from grandparent
        if (uncle && uncle->color == Color::red)
        {
            recolor_parent_uncle_grand_(parent, uncle, grand);
            return grand;
        }

        // node is right child => rotate parent to make a line
        if (!parent->right_is_thread && node == parent->right_)
        {
            node   = parent;
            left_rotate(node);
            parent = get_parent     (node);
            grand  = get_grandparent(node);

            if (!parent || parent == header_ || !grand || grand == header_)
                return node;
        }

        // rotate grand, recolor
        parent->color = Color::black;
        grand ->color = Color::red;
        right_rotate(grand);

        return node;
    }

    // parent is right child of grandparent
    NodeT *fix_insert_parent_is_right_(NodeT *node, NodeT *parent, NodeT *grand) noexcept
    {
        NodeT* uncle = left_child(grand);

        if (uncle && uncle->color == Color::red)
        {
            recolor_parent_uncle_grand_(parent, uncle, grand);
            return grand;
        }

        if (!parent->left_is_thread && node == parent->left_)
        {
            node   = parent;
            right_rotate(node);
            parent = get_parent     (node);
            grand  = get_grandparent(node);
            if (!parent || !grand) return node;
        }

        parent->color = Color::black;
        grand ->color = Color::red;
        left_rotate(grand);

        return node;
    }

    void left_rotate(NodeT *pivot_node) noexcept
    {
        if (!pivot_node)                 return;
        if (pivot_node->right_is_thread) return;

        NodeT *new_root     = pivot_node->right_;
        NodeT *pivot_parent = pivot_node->parent_;
        if (!new_root)                   return;

        if (new_root->left_is_thread)
        {
            pivot_node->right_          = new_root;
            pivot_node->right_is_thread = 1;
        }
        else
        {
            NodeT *beta_subtree         = new_root->left_;
            pivot_node->right_          = beta_subtree;
            pivot_node->right_is_thread = 0;

            if (beta_subtree)
                beta_subtree->parent_   = pivot_node;
        }

        new_root->parent_ = pivot_parent;

        if (pivot_parent == header_)
        {
            root_            = new_root;
            header_->parent_ = root_;
        }
        else if (pivot_node == pivot_parent->left_)
            pivot_parent->left_ = new_root;
        else
            pivot_parent->right_ = new_root;

        new_root->left_          = pivot_node;
        new_root->left_is_thread = 0;
        pivot_node->parent_      = new_root;
    }

    void right_rotate(NodeT *pivot_node) noexcept
    {
        if (!pivot_node)                return;
        if (pivot_node->left_is_thread) return;

        NodeT *new_root     = pivot_node->left_;
        NodeT *pivot_parent = pivot_node->parent_;
        if (!new_root)                  return;

        if (new_root->right_is_thread)
        {
            pivot_node->left_          = new_root;
            pivot_node->left_is_thread = 1;
        }
        else
        {
            NodeT *beta_subtree        = new_root->right_;
            pivot_node->left_          = beta_subtree;
            pivot_node->left_is_thread = 0;

            if (beta_subtree)
                beta_subtree->parent_  = pivot_node;
        }

        new_root->parent_ = pivot_parent;

        if (pivot_parent == header_)
        {
            root_            = new_root;
            header_->parent_ = root_;
        }
        else if (pivot_node == pivot_parent->right_)
            pivot_parent->right_ = new_root;
        else
            pivot_parent->left_ = new_root;



        new_root->right_          = pivot_node;
        new_root->right_is_thread = 0;
        pivot_node->parent_       = new_root;
    }

    static NodeT *leftmost(NodeT *node)
    {
        if (!node)
            return nullptr;
        while (!node->left_is_thread)
            node = node->left_;

        return node;
    }

    NodeT *inorder_successor(NodeT *node) const noexcept
    {
        if (!node)
            return header_;

        if (node->right_is_thread)
            return node->right_;

        node = node->right_;
        while (node && !node->left_is_thread)
            node = node->left_;

        return node ? node : header_;
    }

    void destroy_subtree() noexcept
    {
        if (!header_)
            return;

        NodeT *cur = header_->left_;
        while (cur != header_)
        {
            NodeT *next_node = inorder_successor(cur);
            delete cur;
            cur = next_node;
        }

        root_            = nullptr;
        header_->parent_ = nullptr;
        header_->left_   = header_;
        header_->right_  = header_;
    }


public:
    using const_iterator = RB_const_iterator<KeyT>;

    Red_black_tree()
    {
        header_          = new NodeT();
        header_->color   = Color::black;

        header_->parent_ = nullptr;
        header_->left_   = header_;
        header_->right_  = header_;

        header_->left_is_thread  = 1;
        header_->right_is_thread = 1;

        root_           = nullptr;
    }


    Red_black_tree(KeyT key) : Red_black_tree()
    {
        NodeT *root_node = new NodeT(key, Color::black);

        root_            = root_node;

        header_->parent_ = root_;
        header_->left_   = root_;
        header_->right_  = root_;

        root_->parent_   = header_;

        root_->left_           = header_;
        root_->left_is_thread  = 1;
        root_->right_          = header_;
        root_->right_is_thread = 1;
    }


    ~Red_black_tree()
    {
        destroy_subtree();

        delete header_;
        header_ = nullptr;
    }


    Red_black_tree(const Red_black_tree&)            = delete;
    Red_black_tree &operator=(const Red_black_tree&) = delete;

    Red_black_tree(Red_black_tree&& other) noexcept
        : header_(std::exchange(other.header_, nullptr)),
          root_  (std::exchange(other.root_,   nullptr))
    {
        other.header_          = new NodeT();
        other.header_->color   = Color::black;
        other.header_->parent_ = nullptr;
        other.header_->left_   = other.header_;
        other.header_->right_  = other.header_;

        other.header_->left_is_thread  = 1;
        other.header_->right_is_thread = 1;

        other.root_            = nullptr;
    }

    Red_black_tree &operator=(Red_black_tree &&other) noexcept
    {
        if (this == &other)
            return *this;

        destroy_subtree();
        delete header_;

        header_ = std::exchange(other.header_, nullptr);
        root_   = std::exchange(other.root_,   nullptr);

        other.header_          = new NodeT();
        other.header_->color   = Color::black;
        other.header_->parent_ = nullptr;
        other.header_->left_   = other.header_;
        other.header_->right_  = other.header_;

        other.header_->left_is_thread  = 1;
        other.header_->right_is_thread = 1;

        other.root_            = nullptr;

        return *this;
    }

    // inserting key
    void insert_elem(const KeyT key)
    {
        if (!root_)
        {
            NodeT *new_node = create_red_node_(key, nullptr);
            attach_first_node_(new_node);

            return;
        }

        bool   insert_left = false;
        NodeT *parent_node = find_parent_for_insert_(key, insert_left);

        // key already exists
        if (!parent_node)
            return;

        NodeT *new_node = create_red_node_(key, parent_node);

        if (insert_left)
            attach_as_left_child_(parent_node, new_node);
        else
            attach_as_right_child_(parent_node, new_node);

        fix_insert(new_node);
        enforce_header_threads_();
    }

    const_iterator begin() const
    {
        if (!root_)
            return const_iterator(header_, header_);

        return const_iterator(header_->left_, header_);
    }

    const_iterator end() const
    {
        return const_iterator(header_, header_);
    }

    uint64_t range_queries(const KeyT key1, const KeyT key2) const
    {
        if (key2 <= key1)
            return 0;

        auto first = lower_bound(key1);
        auto last  = upper_bound(key2);

        return std::distance(first, last);
    }


private:
    NodeT *lower_bound_node(const KeyT &key) const
    {
        NodeT *cur = root_;
        NodeT *res = nullptr;

        while (cur)
        {
            if (!(cur->key_ < key)) // key <= cur->key_
            {
                res = cur;
                cur = (cur->left_is_thread ? nullptr : cur->left_);
            }

            else
                cur = (cur->right_is_thread ? nullptr : cur->right_);
        }

        return res;
    }

    NodeT *upper_bound_node(const KeyT &key) const
    {
        NodeT *cur = root_;
        NodeT *res = nullptr;

        while (cur)
        {
            if (key < cur->key_) // cur->key_ > key
            {
                res = cur;
                cur = (cur->left_is_thread ? nullptr : cur->left_);
            }

            else
                cur = (cur->right_is_thread ? nullptr : cur->right_);
        }

        return res;
    }

    NodeT *find_parent_for_insert_(const KeyT key, bool &insert_left) const
    {
        NodeT *parent_node  = nullptr;
        NodeT *current_node = root_;

        while (current_node)
        {
            parent_node = current_node;

            if (key < current_node->key_)
            {
                insert_left = true;
                if (current_node->left_is_thread)
                    break;
                current_node = current_node->left_;
            }
            else if (key > current_node->key_)
            {
                insert_left = false;
                if (current_node->right_is_thread)
                    break;
                current_node = current_node->right_;
            }
            else
            {
                return nullptr; // key already exists
            }
        }

        // if the tree is empty => parent_node == nullptr
        return parent_node;
    }

    NodeT *create_red_node_(const KeyT key, NodeT *parent_node)
    {
        NodeT *new_node = new NodeT(key, Color::red);
               new_node->parent_ = parent_node;

        return new_node;
    }

    void attach_first_node_(NodeT *new_node) noexcept
    {
        root_        = new_node;
        root_->color = Color::black;

        header_->parent_ = root_;
        header_->left_   = root_;
        header_->right_  = root_;

        root_->parent_   = header_;

        root_->left_           = header_;
        root_->left_is_thread  = 1;
        root_->right_          = header_;
        root_->right_is_thread = 1;
    }

    void attach_as_left_child_(NodeT *parent_node, NodeT *new_node) noexcept
    {
        new_node->left_           = parent_node->left_;
        new_node->left_is_thread  = 1;
        new_node->right_          = parent_node;
        new_node->right_is_thread = 1;

        parent_node->left_          = new_node;
        parent_node->left_is_thread = 0;

        NodeT *predecessor_node = new_node->left_;
        if (predecessor_node == header_)
        {
            header_->left_ = new_node;
        }
        else if (predecessor_node && predecessor_node->right_is_thread)
        {
            predecessor_node->right_ = new_node;
        }
    }

    void attach_as_right_child_(NodeT *parent_node, NodeT *new_node) noexcept
    {
        new_node->right_          = parent_node->right_;
        new_node->right_is_thread = 1;
        new_node->left_           = parent_node;
        new_node->left_is_thread  = 1;

        parent_node->right_          = new_node;
        parent_node->right_is_thread = 0;

        NodeT *successor_node = new_node->right_;
        if (successor_node == header_)
        {
            header_->right_ = new_node;
        }
        else if (successor_node && successor_node->left_is_thread)
        {
            successor_node->left_ = new_node;
        }
    }

    void enforce_header_threads_() noexcept
    {
        if (header_->left_ != header_)
        {
            header_->left_->left_          = header_;
            header_->left_->left_is_thread = 1;
        }

        if (header_->right_ != header_)
        {
            header_->right_->right_          = header_;
            header_->right_->right_is_thread = 1;
        }
    }

public:
#ifdef CUSTOM_MODE_DEBUG
    const NodeT *debug_root() const noexcept { return root_; } // for internal debugging tools only
#endif

    const_iterator lower_bound(const KeyT &key) const
    {
        NodeT *found_node = lower_bound_node(key);
        return const_iterator(found_node ? found_node : header_, header_);
    }

    const_iterator upper_bound(const KeyT &key) const
    {
        NodeT *found_node = upper_bound_node(key);
        return const_iterator(found_node ? found_node : header_, header_);
    }
};

}; // namespace Tree