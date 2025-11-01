#include <gtest/gtest.h>
#include <memory>
#include "red_black_tree.hpp"

using Key = int64_t;

template <class KeyT>
static int CheckRB(const Tree::Node<KeyT>* n) 
{
    if (!n) return 1; 

    if (n->color == Tree::Color::red) 
    {
        if (n->left_ ) EXPECT_EQ(n->left_->color,  Tree::Color::black);
        if (n->right_) EXPECT_EQ(n->right_->color, Tree::Color::black);
    }

    int lh = CheckRB(n->left_ );
    int rh = CheckRB(n->right_);
    EXPECT_EQ(lh, rh) << "black-height mismatch";

    return lh + (n->color == Tree::Color::black ? 1 : 0);
}


TEST(RBTreeUnit, EmptyTreeRangeIsZero){
    Tree::Red_black_tree<Key> t;
    EXPECT_EQ(t.range_queries(0, 10), 0);
    EXPECT_EQ(t.range_queries(10, 0), 0); 
    
}

TEST(RBTreeUnit, SingleInsert) {
    Tree::Red_black_tree<Key> t;
    t.insert_elem(5);
    EXPECT_EQ(t.range_queries(0, 10), 1);
    EXPECT_EQ(t.range_queries(6, 10), 0);
}

TEST(RBTreeUnit, DuplicatesIgnored) {
    Tree::Red_black_tree<Key> t;
    t.insert_elem(10);
    t.insert_elem(10);
    t.insert_elem(10);
    EXPECT_EQ(t.range_queries(10, 10), 1);
}

TEST(RBTreeUnit, BordersInclusive) {
    Tree::Red_black_tree<Key> t;
    for (Key x : {10, 20, 30}) t.insert_elem(x);
    EXPECT_EQ(t.range_queries(10, 10), 1);
    EXPECT_EQ(t.range_queries(20, 30), 2);
    EXPECT_EQ(t.range_queries(9, 31), 3);
}

TEST(RBTreeUnit, RBInvariantsAfterInsert) {
    Tree::Red_black_tree<Key> t;
    for (Key x : {10, 20, 30, 15, 25, 5, 1, 50, 60, 55, 54})
        t.insert_elem(x);

    auto root = t.get_root();
    ASSERT_TRUE(root);
    EXPECT_EQ(root->color, Tree::Color::black);
    (void)CheckRB(root);
}
