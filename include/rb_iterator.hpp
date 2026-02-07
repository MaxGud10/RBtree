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

    const NodeT *node_ = nullptr;
    const NodeT *root_ = nullptr;

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
        : node_(node), root_(root) {}

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
        if (!node_) return *this;

        if (node_->right_)
        {
            node_ = leftmost(node_->right_);

            return *this;
        }

        auto cur = node_;
        auto p   = node_->parent_;

        while (p && cur == p->right_)
        {
            cur = p;
            p   = p->parent_;
        }

        node_ = p;
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
            node_ = rightmost(root_);
            return *this;
        }

        if (node_->left_)
        {
            node_ = rightmost(node_->left_);
            return *this;
        }

        auto cur = node_;
        auto p   = node_->parent_;

        while (p && cur == p->left_)
        {
            cur = p;
            p   = p->parent_;
        }

        node_ = p;
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
