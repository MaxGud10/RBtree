#pragma once

#include <cstddef>

namespace Tree
{

template <typename KeyT>
struct Node; 

template <typename KeyT>
class RB_Iterator
{
    Node<KeyT>* node_ = nullptr;

    Node<KeyT>* leftmost(Node<KeyT>* n)
    {
        if (!n) 
            return nullptr;

        while (n->left_) 
            n = n->left_;
            
        return n;
    }

    Node<KeyT>* rightmost(Node<KeyT>* n)
    {
        if (!n) 
            return nullptr;

        while (n->right_) 
            n = n->right_;

        return n;
    }

public:
             RB_Iterator() = default;
    explicit RB_Iterator(Node<KeyT>* node) : node_(node) {}

    const KeyT& operator* () const { return node_->key_; }
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

// TODO: если std:dist будет норм работать, то закоментить
template <typename It>
std::size_t my_distance(It first, It last)
{
    std::size_t cnt = 0;
    for (; first != last; ++first)
        ++cnt;

    return cnt;
}

} // namespace Tree
