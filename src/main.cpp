#include <iostream>
#include <cstdint>
#include <set>
#include <string>
#include "cxxopts.hpp"

#include "red_black_tree.hpp"
#include "graphic_dump.hpp"


#ifdef SET_MODE_ENABLED
constexpr bool kVerifyWithSet = true;
#else
constexpr bool kVerifyWithSet = false;
#endif


static std::string get_gv_file_arg(int argc, char** argv, const char* def_name)
{
    cxxopts::Options options("rb_tree", "Red-black tree visualizer");
    options.add_options()
        ("f,gv-file",   "Path to .dot output file", cxxopts::value<std::string>())
        ("p,gv-prefix", "Prefix for .dot file name", cxxopts::value<std::string>())
        ("h,help",      "Print help");

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        std::exit(0);
    }

    if (result.count("gv-file"))
        return result["gv-file"].as<std::string>();

    if (result.count("gv-prefix"))
        return result["gv-prefix"].as<std::string>() + "_tree.dot";

    return def_name;
}

static int64_t     range_queries_set(const std::set<int64_t>& set, int64_t a, int64_t b);
static bool        read_next        (std::istream& in, char& mode, int64_t& a, int64_t& b);
static inline void verify_query     (const std::set<int64_t>& set, int64_t a, int64_t b, int64_t ans);
static void        run_normal       (int argc, char** argv);


int main(int argc, char** argv)
{
    run_normal(argc, argv);

    return 0;
}

int64_t range_queries_set(const std::set<int64_t>& set, int64_t a, int64_t b)
{
    if (b < a) 
        return 0;

    auto first = set.lower_bound(a);
    auto last  = set.upper_bound(b);

    return std::distance(first, last);
}

static bool read_next(std::istream& in, char& mode, int64_t& a, int64_t& b)
{
    if (!(in >> mode)) return false;

    if (mode == 'k') 
    {
        if (!(in >> a)) 
        {
            std::cerr << "ERROR: expected key after 'k'\n";
            return false;
        }
        return true;
    }

    if (mode == 'q') 
    {
        if (!(in >> a >> b)) 
        {
            std::cerr << "ERROR: expected two numbers after 'q'\n";
            return false;
        }

        return true;
    }

    std::cerr << "ERROR: unknown mode: '" << mode << "'\n";
    return false;
}

// если режим выключен, компилятор выкинет код этой функции 
static inline void verify_query(const std::set<int64_t>& set, int64_t a, int64_t b, int64_t ans)
{
    const auto check = range_queries_set(set, a, b);
    if (check != ans) 
    {
        std::cerr << "DBG set=" << check
                  << " tree=" << ans
                  << " for q " << a << ' ' << b << '\n';
    }
}

static void run_normal(int argc, char** argv)
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Tree::Red_black_tree<int64_t> tree;

    std::set<int64_t> set;


    char    mode;
    int64_t a = 0;
    int64_t b = 0;

    bool printed_any_answer = false;

    while (read_next(std::cin, mode, a, b))
    {
        if (mode == 'k') 
        {
            tree.insert_elem(a);

            if constexpr (kVerifyWithSet)
            {
                set.insert(a);
            }

            continue;
        }

        // mode == 'q'
        const auto ans = tree.range_queries(a, b);
        std::cout << ans << ' ';
        printed_any_answer = true;

        if constexpr (kVerifyWithSet)
        {
            verify_query(set, a, b, ans);
        }
    }

    if (printed_any_answer) std::cout << '\n';

#ifdef CUSTOM_MODE_DEBUG
    {
        const std::string dot_name = get_gv_file_arg(argc, argv, "graphviz/file_graph.dot");
        Tree::Print_tree<int64_t> pr_tr;
        pr_tr.dump(tree, dot_name.c_str(), "graphviz/tree_graph.png", true);
    }
#endif
}

