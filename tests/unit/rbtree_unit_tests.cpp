#include <gtest/gtest.h>
#include <unordered_map>
#include <optional>

#include "red_black_tree.hpp"

using Key = int64_t;

#ifdef CUSTOM_MODE_DEBUG

struct Info {
    Tree::Color color{};
    std::optional<Key> parent;
    std::optional<Key> left;
    std::optional<Key> right;
};

static int CheckRBRec(const std::unordered_map<Key, Info>& m, std::optional<Key> k)
{
    if (!k) return 1;

    const auto it = m.find(*k);
    EXPECT_TRUE(it != m.end());
    if (it == m.end()) return 1;

    const Info& n = it->second;

    if (n.color == Tree::Color::red) {
        if (n.left)  EXPECT_EQ(m.at(*n.left).color,  Tree::Color::black);
        if (n.right) EXPECT_EQ(m.at(*n.right).color, Tree::Color::black);
    }

    const int lh = CheckRBRec(m, n.left);
    const int rh = CheckRBRec(m, n.right);
    EXPECT_EQ(lh, rh) << "black-height mismatch at key=" << *k;

    return lh + (n.color == Tree::Color::black ? 1 : 0);
}

#endif


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

#ifdef CUSTOM_MODE_DEBUG
    std::unordered_map<Key, Info> m;
    std::optional<Key> root_key;

    t.debug_visit([&](const Key&         key,
                      Tree::Color        color,
                      std::optional<Key> parent,
                      std::optional<Key> left,
                      std::optional<Key> right)
    {
        m[key] = Info{color, parent, left, right};
        if (!parent) root_key = key;
    });

    ASSERT_TRUE(root_key.has_value()) << "tree should have a root";
    ASSERT_TRUE(m.count(*root_key));

    EXPECT_EQ(m.at(*root_key).color, Tree::Color::black) << "root must be black";
    (void)CheckRBRec(m, root_key);
#else
    GTEST_SKIP() << "RB invariants check requires CUSTOM_MODE_DEBUG";
#endif
}
