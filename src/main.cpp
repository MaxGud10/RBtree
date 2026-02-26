#include <iostream>
#include <cstdint>
#include <set>
#include <string>

#include "cxxopts.hpp"
#include "red_black_tree.hpp"
#include "graphic_dump.hpp"
#include "driver.hpp"

using TreeT = Tree::Red_black_tree<int64_t>;

static std::string get_gv_file_arg(int argc, char** argv, const char* def_name)
{
    cxxopts::Options options("rb_tree", "Red-black tree visualizer");

    options.add_options()
        ("f,gv-file",   "Path to .dot output file",  cxxopts::value<std::string>())
        ("p,gv-prefix", "Prefix for .dot file name", cxxopts::value<std::string>())
        ("h,help",      "Print help");

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        std::exit(0);
    }

    if (result.count("gv-file"))
        return result["gv-file"].as<std::string>();

    if (result.count("gv-prefix"))
        return result["gv-prefix"].as<std::string>() + "_tree.dot";

    return def_name;
}

struct Normal_policy
{
    std::set<int64_t> ref_;
    bool printed_any_ = false;

    void insert(TreeT &tree, int64_t key)
    {
        tree.insert_elem(key);

        if constexpr (Driver::kVerifyWithSet)
            ref_.insert(key);
    }

    int64_t query(TreeT &tree, int64_t a, int64_t b)
    {
        return tree.range_queries(a, b);
    }

    void handle_answer(int64_t a, int64_t b, int64_t ans)
    {
        std::cout << ans << ' ';
        printed_any_ = true;

        if constexpr (Driver::kVerifyWithSet)
        {
            auto first = ref_.lower_bound(a);
            auto last  = ref_.upper_bound(b);

            const auto check = std::distance(first, last);

            if (check != ans)
            {
                std::cerr << "DBG set=" << check
                          << " tree=" << ans
                          << " for q " << a << ' ' << b << '\n';
            }
        }
    }

    void finalize()
    {
        if (printed_any_)
            std::cout << '\n';
    }
};

int main(int argc, char** argv)
{
    TreeT tree;
    Normal_policy policy;

    const int rc = Driver::run(tree, policy);

#ifdef CUSTOM_MODE_DEBUG
    {
        const std::string dot_name =
            get_gv_file_arg(argc, argv, "graphviz/file_graph.dot");

        Tree::Print_tree<int64_t> pr_tr;
        pr_tr.dump(tree, dot_name.c_str(), "graphviz/tree_graph.png", true);
    }
#endif

    return rc;
}