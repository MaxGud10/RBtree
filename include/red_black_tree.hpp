#pragma once

#include <cstdlib>
#include <utility>
#include <iterator>
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

    Node* parent_ = nullptr;
    Node* left_   = nullptr;
    Node* right_  = nullptr;

    Node() = default;

    Node(const KeyT &key, Color color = Color::red) : key_{key}, color{color} {}
};
} // namespace detail

template <typename KeyT>
class Red_black_tree
{
    using NodeT  = detail::Node<KeyT>;

    NodeT *root_ = nullptr;

    NodeT *get_parent(NodeT *node) const
    {
        return node ? node->parent_ : nullptr;
    }

    NodeT *get_grandparent(NodeT *node) const
    {
        NodeT *parent_node = get_parent(node);

        return parent_node ? parent_node->parent_ : nullptr;
    }

    // balance after insert
    void fix_insert(NodeT* node)
    {
        while (node && node != root_)
        {
            NodeT* parent = get_parent(node);
            if (!parent || parent->color != Color::red)
                break;

            NodeT* grand = get_grandparent(node);
            if (!grand)
                break;

            if (parent == grand->left_)
                node = fix_insert_parent_is_left_(node, parent, grand);
            else
                node = fix_insert_parent_is_right_(node, parent, grand);
        }

        if (root_)
            root_->color = Color::black;
    }

    void recolor_parent_uncle_grand_(NodeT* parent, NodeT* uncle, NodeT* grand)
    {
        parent->color = Color::black;
        uncle->color  = Color::black;
        grand->color  = Color::red;
    }

    // parent is left child of grandparent
    NodeT* fix_insert_parent_is_left_(NodeT* node, NodeT* parent, NodeT* grand)
    {
        NodeT* uncle = grand->right_;

        // uncle is red => recolor and continue from grandparent
        if (uncle && uncle->color == Color::red)
        {
            recolor_parent_uncle_grand_(parent, uncle, grand);
            return grand;
        }

        // node is right child => rotate parent to make a line
        if (node == parent->right_)
        {
            node = parent;
            left_rotate(node);
            parent = get_parent(node);
            grand  = get_grandparent(node);
            if (!parent || !grand) return node;
        }

        // rotate grand, recolor
        parent->color = Color::black;
        grand->color  = Color::red;
        right_rotate(grand);

        return node;
    }

    // parent is right child of grandparent
    NodeT* fix_insert_parent_is_right_(NodeT* node, NodeT* parent, NodeT* grand)
    {
        NodeT* uncle = grand->left_;

        if (uncle && uncle->color == Color::red)
        {
            recolor_parent_uncle_grand_(parent, uncle, grand);
            return grand;
        }

        if (node == parent->left_)
        {
            node = parent;
            right_rotate(node);
            parent = get_parent(node);
            grand  = get_grandparent(node);
            if (!parent || !grand) return node;
        }

        parent->color = Color::black;
        grand->color  = Color::red;
        left_rotate(grand);

        return node;
    }

    void left_rotate(NodeT *pivot_node)
    {
        if (!pivot_node)
            return;

        NodeT *new_root = pivot_node->right_;

        if (!new_root)
            return;

        pivot_node->right_ = new_root->left_;
        if (new_root->left_)
            new_root->left_->parent_ = pivot_node;

        NodeT *parent_of_pivot  = pivot_node->parent_;
        new_root->parent_       = parent_of_pivot;

        if (!parent_of_pivot)
            root_ = new_root;

        else if (pivot_node == parent_of_pivot->left_)
            parent_of_pivot->left_  = new_root;

        else
            parent_of_pivot->right_ = new_root;

        new_root->left_ = pivot_node;
                          pivot_node->parent_ = new_root;
    }

    void right_rotate(NodeT *pivot_node)
    {
        if (!pivot_node)
            return;

        NodeT *new_root = pivot_node->left_;

        if (!new_root)
            return;

        pivot_node->left_ = new_root->right_;
        if (new_root->right_)
            new_root->right_->parent_ = pivot_node;

        NodeT *parent_of_pivot = pivot_node->parent_;
        new_root->parent_      = parent_of_pivot;

        if (!parent_of_pivot)
            root_ = new_root;

        else if (pivot_node == parent_of_pivot->right_)
            parent_of_pivot->right_ = new_root;

        else
            parent_of_pivot->left_  = new_root;

        new_root->right_ = pivot_node;
                           pivot_node->parent_ = new_root;
    }

    static void destroy_subtree(NodeT* root) noexcept
    {
        if (!root)
            return;

        std::vector<NodeT*> st;
        st.reserve(64);

        NodeT* cur          = root;
        NodeT* last_visited = nullptr;

        while (cur || !st.empty())
        {
            if (cur)
            {
                st.push_back(cur);
                cur = cur->left_;
            }

            else
            {
                NodeT* peek = st.back();

                if (peek->right_ && last_visited != peek->right_)
                    cur = peek->right_;
                else
                {
                    st.pop_back();
                    last_visited = peek;
                    
                    delete peek;
                }
            }
        }
    }


    // static void destroy_subtree(NodeT *node) noexcept
    // {
    //     if (!node)
    //         return;

    //     destroy_subtree(node->left_);
    //     destroy_subtree(node->right_);

    //     delete node;
    // }

public:
    using const_iterator = RB_const_iterator<KeyT>;

    Red_black_tree() = default;

    Red_black_tree(KeyT key)
    {
        root_ = new NodeT (key, Color::black);
    }

    ~Red_black_tree()
    {
        destroy_subtree(root_);
        root_ = nullptr;
    }

    Red_black_tree(const Red_black_tree&)            = delete;
    Red_black_tree &operator=(const Red_black_tree&) = delete;

    Red_black_tree(Red_black_tree&& other) noexcept : root_(std::exchange(other.root_, nullptr)) {}

    Red_black_tree &operator=(Red_black_tree&& other) noexcept
    {
        if (this != &other)
        {
            destroy_subtree(root_);

            root_ = std::exchange(other.root_, nullptr);
        }

        return *this;
    }

    // вставка ключа
    void insert_elem(const KeyT key)
    {
        NodeT *parent  = nullptr;
        NodeT *current = root_;

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

        // cоздаем новый узел и привязать
        NodeT *new_node = new NodeT(key, Color::red);
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

        try
        {
            fix_insert(new_node);
        }
        catch (...)
        {
            if (parent->left_ == new_node)
                parent->left_ = nullptr;

            else if (parent->right_ == new_node)
                parent->right_ = nullptr;

            delete new_node;

            throw;
        }
    }

    const_iterator begin() const
    {
        const NodeT *node = root_;
        if (!node)
            return const_iterator(nullptr, root_);

        while (node->left_)
            node = node->left_;

        return const_iterator(node, root_);
    }

    const_iterator end() const
    {
        return const_iterator(nullptr, root_);
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
    NodeT *lower_bound_node(const KeyT& key) const
    {
        NodeT *cur = root_;
        NodeT *res = nullptr;

        while (cur)
        {
            if (!(cur->key_ < key)) // key <= cur->key_
            {
                res = cur;
                cur = cur->left_;
            }

            else
                cur = cur->right_;
        }

        return res;
    }

    NodeT *upper_bound_node(const KeyT& key) const
    {
        NodeT *cur = root_;
        NodeT *res = nullptr;

        while (cur)
        {
            if (key < cur->key_) // cur->key_ > key
            {
                res = cur;
                cur = cur->left_;
            }

            else
                cur = cur->right_;
        }

        return res;
    }

public:
#ifdef CUSTOM_MODE_DEBUG
    const NodeT* debug_root() const noexcept { return root_; } // только для internal debug tools
#endif

    const_iterator lower_bound(const KeyT& key) const
    {
        return const_iterator(lower_bound_node(key), root_);
    }

    const_iterator upper_bound(const KeyT& key) const
    {
        return const_iterator(upper_bound_node(key), root_);
    }
};

}; // namespace Tree
