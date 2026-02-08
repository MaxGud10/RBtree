#include <gtest/gtest.h>
#include <unordered_map>
#include <optional>

#include "red_black_tree.hpp"

using Key = int64_t;

#ifdef CUSTOM_MODE_DEBUG

using NodeT = Tree::detail::Node<Key>;

struct Info {
    Tree::Color color{};
    std::optional<Key> parent;
    std::optional<Key> left;
    std::optional<Key> right;
};

static int CheckRBRec(const NodeT* n,
                      std::optional<Key> min_key = std::nullopt,
                      std::optional<Key> max_key = std::nullopt)
{
    if (!n) return 1;

    if (min_key) EXPECT_TRUE(*min_key < n->key_) << "BST violation: key <= min";
    if (max_key) EXPECT_TRUE(n->key_ < *max_key) << "BST violation: key >= max";

    if (n->color == Tree::Color::red) {
        if (n->left_)  EXPECT_EQ(n->left_->color,  Tree::Color::black);
        if (n->right_) EXPECT_EQ(n->right_->color, Tree::Color::black);
    }

    const int lh = CheckRBRec(n->left_,  min_key, n->key_);
    const int rh = CheckRBRec(n->right_, n->key_, max_key);

    EXPECT_EQ(lh, rh) << "black-height mismatch at key=" << n->key_;

    return lh + (n->color == Tree::Color::black ? 1 : 0);
}


#endif

TEST(RBTreeUnit, EmptyTreeRangeIsZero){
    Tree::Red_black_tree<Key> t;
    EXPECT_EQ(t.range_queries(0, 10), 0);
    EXPECT_EQ(t.range_queries(10, 0), 0);
    EXPECT_EQ(t.range_queries(5, 5), 0);
}

TEST(RBTreeUnit, SingleInsert) {
    Tree::Red_black_tree<Key> t;
    t.insert_elem(5);

    EXPECT_EQ(t.range_queries(0, 10), 1);
    EXPECT_EQ(t.range_queries(6, 10), 0);

    EXPECT_EQ(t.range_queries(5, 5), 0);
    EXPECT_EQ(t.range_queries(4, 5), 1);
    EXPECT_EQ(t.range_queries(4, 6), 1);
}

TEST(RBTreeUnit, DuplicatesIgnored) {
    Tree::Red_black_tree<Key> t;
    t.insert_elem(10);
    t.insert_elem(10);
    t.insert_elem(10);

    EXPECT_EQ(t.range_queries(10, 10), 0);
    EXPECT_EQ(t.range_queries(9, 10), 1);
    EXPECT_EQ(t.range_queries(9, 11), 1);
}

TEST(RBTreeUnit, BordersInclusive) {
    Tree::Red_black_tree<Key> t;
    for (Key x : {10, 20, 30}) t.insert_elem(x);

    EXPECT_EQ(t.range_queries(10, 10), 0);
    EXPECT_EQ(t.range_queries(9, 10), 1);
    EXPECT_EQ(t.range_queries(10, 11), 1);
    EXPECT_EQ(t.range_queries(20, 30), 2);
    EXPECT_EQ(t.range_queries(9, 31), 3);
}

TEST(RBTreeUnit, RBInvariantsAfterInsert)
{
    Tree::Red_black_tree<Key> t;
    for (Key x : {10, 20, 30, 15, 25, 5, 1, 50, 60, 55, 54})
        t.insert_elem(x);

#ifdef CUSTOM_MODE_DEBUG
    const NodeT* root = t.debug_root();
    ASSERT_TRUE(root != nullptr);

    EXPECT_EQ(root->color, Tree::Color::black) << "root must be black";
    (void)CheckRBRec(root);
#else
    GTEST_SKIP() << "RB invariants check requires CUSTOM_MODE_DEBUG";
#endif
}
