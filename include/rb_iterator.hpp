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
    const NodeT* min_  = nullptr;
    const NodeT* max_  = nullptr;

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
    RB_const_iterator(NodeT *node, const NodeT *minv, const NodeT *maxv)
        : node_(node), min_(minv), max_(maxv) {}

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

        node_ = node_->next_;
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
            node_  = max_;
            return *this;
        }

        assert(node_ != min_ && "--begin() is UB");
        node_ = node_->prev_;

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
