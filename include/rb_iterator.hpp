#pragma once

#include <cstddef>
#include <iterator>

namespace Tree
{

template <typename KeyT>
struct Node; 

template <typename KeyT>
class RB_Iterator
{
    Node<KeyT>* node_ = nullptr;

    Node<KeyT>* leftmost(Node<KeyT>* n) const
    {
        if (!n) 
            return nullptr;

        while (n->left_) 
            n = n->left_;
            
        return n;
    }

    Node<KeyT>* rightmost(Node<KeyT>* n) const 
    {
        if (!n) 
            return nullptr;

        while (n->right_) 
            n = n->right_;

        return n;
    }

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = KeyT;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const KeyT*; // [x]
    using reference         = const KeyT&; // [x]

             RB_Iterator() = default;
    explicit RB_Iterator(Node<KeyT>* node) : node_(node) {}

    const KeyT& operator* () const { return  node_->key_; }
    const KeyT* operator->() const { return &node_->key_; }

    bool operator==(const RB_Iterator& other) const { return node_ == other.node_; }
    bool operator!=(const RB_Iterator& other) const { return node_ != other.node_; }

    RB_Iterator& operator++()
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

    RB_Iterator& operator--()
    {
        if (!node_) return *this;

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

    Node<KeyT>* get_node() const { return node_; }
};

} // namespace Tree
