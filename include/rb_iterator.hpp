#pragma once

#include <cstddef>
#include <iterator>
#include <cassert>

namespace Tree
{

namespace detail
{
template <typename KeyT>
struct Node;
}

template <typename KeyT>
class RB_const_iterator
{
    using NodeT = detail::Node<KeyT>;

    const NodeT *root_ = nullptr;
    const NodeT *node_ = nullptr;

    const NodeT *leftmost(const NodeT *n) const
    {
        while (n && n->left_)
            n = n->left_;

        return n;
    }

    const NodeT *rightmost(const NodeT *n) const
    {
        while (n && n->right_)
            n = n->right_;

        return n;
    }

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = KeyT;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const KeyT*;
    using reference         = const KeyT&;

    RB_const_iterator() = default;
    RB_const_iterator(NodeT *node, const NodeT *root)
        : root_(root), node_(node) {}

    reference operator* () const
    {
        assert(node_ && "dereferencing end() iterator");
        return node_->key_;
    }

    pointer operator->() const
    {
        assert(node_ && "dereferencing end() iterator");
        return &node_->key_;
    }

    bool operator==(const RB_const_iterator &other) const { return node_ == other.node_; }
    bool operator!=(const RB_const_iterator &other) const { return node_ != other.node_; }

    // ++it
    RB_const_iterator &operator++()
    {
        assert(node_ && "++end() is UB");

        if (node_->right_thread_)
            node_ = node_->right_;

        else
        {
            node_ = node_->right_;
            while (node_ && !node_->left_thread_)
                node_ = node_->left_;
        }

        return *this;
    }

    // it++
    RB_const_iterator operator++(int)
    {
        RB_const_iterator tmp = *this;
        ++(*this);

        return tmp;
    }

    // --it
    RB_const_iterator &operator--()
    {
        if (!node_)
        {
            node_ = root_;

            if (!node_)
                return *this;

            while (!node_->right_thread_)
                node_ = node_->right_;

            return *this;
        }

        if (node_->left_thread_)
            node_ = node_->left_;

        else
        {
            node_ = node_->left_;
            while (node_ && !node_->right_thread_)
                node_ = node_->right_;
        }

        return *this;
    }

    // it--
    RB_const_iterator operator--(int)
    {
        RB_const_iterator tmp = *this;
        --(*this);

        return tmp;
    }

    const NodeT *get_node() const { return node_; }
};

} // namespace Tree
