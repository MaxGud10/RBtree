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

    const NodeT *node_   = nullptr;
    const NodeT *header_ = nullptr;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = KeyT;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const KeyT*;
    using reference         = const KeyT&;

    RB_const_iterator() = default;
    RB_const_iterator(NodeT *node, const NodeT *header)
        : node_(node), header_(header) {}

    reference operator* () const
    {
        assert(node_ != header_ && "dereferencing end() iterator");
        return node_->key_;
    }

    pointer operator->() const
    {
        assert(node_ != header_ && "dereferencing end() iterator");
        return &node_->key_;
    }

    bool operator==(const RB_const_iterator &other) const { return node_ == other.node_; }
    bool operator!=(const RB_const_iterator &other) const { return node_ != other.node_; }

    // ++it
    RB_const_iterator &operator++()
    {
        assert(node_ != header_ && "++end() is UB");

        if (node_->right_is_thread)
        {
            node_ = node_->right_;
            return *this;
        }

        node_ = node_->right_;
        while (node_ != header_ && !node_->left_is_thread)
            node_ = node_->left_;

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
        if (node_ == header_)
        {
            node_ = header_->right_;
            return *this;
        }

        assert(node_ != header_->left_ && "--begin() is UB");

        if (node_->left_is_thread)
        {
            node_ = node_->left_;
            return *this;
        }

        node_ = node_->left_;
        while (node_ != header_ && !node_->right_is_thread)
            node_ = node_->right_;

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