#include <gtest/gtest.h>
#include <unordered_map>
#include <optional>

#include <atomic>
#include <mutex>
#include <unordered_set>
#include <new>
#include <vector>

#include "red_black_tree.hpp"

using Key   = int64_t;
using NodeT = Tree::detail::Node<Key>;

#ifdef CUSTOM_MODE_DEBUG

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

    const NodeT* left_child  = (!n->left_is_thread  ? n->left_  : nullptr);
    const NodeT* right_child = (!n->right_is_thread ? n->right_ : nullptr);

    if (min_key) EXPECT_TRUE(*min_key < n->key_) << "BST violation: key <= min";
    if (max_key) EXPECT_TRUE(n->key_ < *max_key) << "BST violation: key >= max";

    if (n->color == Tree::Color::red)
    {
        if (left_child)  EXPECT_EQ(left_child->color,  Tree::Color::black);
        if (right_child) EXPECT_EQ(right_child->color, Tree::Color::black);
    }

    const int lh = CheckRBRec(left_child,  min_key, n->key_);
    const int rh = CheckRBRec(right_child, n->key_, max_key);

    EXPECT_EQ(lh, rh) << "black-height mismatch at key=" << n->key_;

    return lh + (n->color == Tree::Color::black ? 1 : 0);
}
#endif


struct ThrowingKey
{
    int64_t v = 0;

    static inline int copies_left = 1'000'000;
    static inline int compares_left = 1'000'000;

    ThrowingKey() = default;
    explicit ThrowingKey(int64_t x) : v(x) {}

    ThrowingKey(const ThrowingKey& other) : v(other.v)
    {
        if (--copies_left == 0) throw std::runtime_error("copy fail");
    }

    ThrowingKey& operator=(const ThrowingKey& other)
    {
        if (this == &other) return *this;
        if (--copies_left == 0) throw std::runtime_error("assign fail");
        v = other.v;
        return *this;
    }

    friend bool operator<(const ThrowingKey& a, const ThrowingKey& b)
    {
        if (--compares_left == 0) throw std::runtime_error("compare fail");
        return a.v < b.v;
    }

    friend bool operator>(const ThrowingKey& a, const ThrowingKey& b)
    {
        if (--compares_left == 0) throw std::runtime_error("compare fail");
        return a.v > b.v;
    }
};

static std::vector<int64_t> DumpKeys(const Tree::Red_black_tree<ThrowingKey>& t)
{
    std::vector<int64_t> v;
    for (auto it = t.begin(); it != t.end(); ++it)
        v.push_back((*it).v);
    return v;
}



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


TEST(RBTreeUnit, ExceptionSafety_InsertStrongGuarantee_OnKeyCopyThrow)
{
    Tree::Red_black_tree<ThrowingKey> t;
    for (int64_t x : {10, 20, 30, 15, 25})
        t.insert_elem(ThrowingKey{x});

    auto before = DumpKeys(t);

    ThrowingKey::copies_left   = 1;
    ThrowingKey::compares_left = 1'000'000;

    EXPECT_THROW(t.insert_elem(ThrowingKey{17}), std::runtime_error);

    EXPECT_EQ(DumpKeys(t), before);
}

TEST(RBTreeUnit, ExceptionSafety_InsertStrongGuarantee_OnCompareThrow)
{
    Tree::Red_black_tree<ThrowingKey> t;
    for (int64_t x : {10, 20, 30, 15, 25})
        t.insert_elem(ThrowingKey{x});

    auto before = DumpKeys(t);

    ThrowingKey::copies_left = 1'000'000;
    ThrowingKey::compares_left = 1;

    EXPECT_THROW(t.insert_elem(ThrowingKey{17}), std::runtime_error);

    EXPECT_EQ(DumpKeys(t), before);
}

TEST(RBTreeUnit, ExceptionSafety_CopyAssign_StrongGuarantee_OnKeyCopyThrow)
{
    Tree::Red_black_tree<ThrowingKey> a;
    for (int64_t x : {1, 2, 3, 4, 5})
        a.insert_elem(ThrowingKey{x});

    Tree::Red_black_tree<ThrowingKey> b;
    for (int64_t x : {10, 20, 30, 40, 50, 60})
        b.insert_elem(ThrowingKey{x});

    auto a_before = DumpKeys(a);

    ThrowingKey::copies_left = 3;
    ThrowingKey::compares_left = 1'000'000;

    EXPECT_THROW(a = b, std::runtime_error);
    EXPECT_EQ(DumpKeys(a), a_before) << "a must remain unchanged on failed assignment";
}

TEST(RBTreeUnit, ExceptionSafety_CopyCtor_OnKeyCopyThrow)
{
    Tree::Red_black_tree<ThrowingKey> src;
    for (int64_t x : {10, 20, 30, 15, 25, 5, 1})
        src.insert_elem(ThrowingKey{x});

    ThrowingKey::copies_left   = 3;
    ThrowingKey::compares_left = 1'000'000;

    EXPECT_THROW((Tree::Red_black_tree<ThrowingKey>(src)), std::runtime_error);


    ThrowingKey::copies_left   = 1'000'000;
    ThrowingKey::compares_left = 1'000'000;

    auto keys = DumpKeys(src);
    EXPECT_EQ(keys.size(), 7u);
    EXPECT_EQ(keys.front(), 1);
    EXPECT_EQ(keys.back(), 30);
}