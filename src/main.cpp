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


// засунуть в nix 
// cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSET_MODE_ENABLED=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=YES

// // TODo: nix 

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

static int64_t range_queries_set(const std::set<int64_t>& set, int64_t a, int64_t b);
static bool    read_next        (std::istream& in, char& mode, int64_t& a, int64_t& b);
static void    run_normal       (bool verify_with_set, int argc, char** argv);

int main(int argc, char** argv)
{
    run_normal(kVerifyWithSet, argc, argv);

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

#ifdef SET_MODE_ENABLED
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
#endif


static void run_normal(bool verify_with_set, int argc, char** argv)
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    RangeQueries::Range_queries<int64_t> range_quer;
#ifdef SET_MODE_ENABLED
    std::set<int64_t> set;
#endif

    char    mode;
    int64_t a = 0;
    int64_t b = 0;

    bool printed_any_answer = false;

    while (read_next(std::cin, mode, a, b))
    {
        if (mode == 'k') 
        {
            range_quer.add_element(a);
#ifdef SET_MODE_ENABLED
            if (verify_with_set) set.insert(a);
#endif
            continue;
        }

        // mode == 'q'
        const auto ans = range_quer.find_range_elements(a, b);
        std::cout << ans << ' ';
        printed_any_answer = true;

#ifdef SET_MODE_ENABLED
        if (verify_with_set) 
            verify_query(set, a, b, ans);
#endif
    }

    if (printed_any_answer) std::cout << '\n';

#ifdef CUSTOM_MODE_DEBUG
    {
        const std::string dot_name = get_gv_file_arg(argc, argv, "graphviz/file_graph.dot");
        Tree::Print_tree<int64_t> pr_tr;
        pr_tr.dump(range_quer.get_tree(), dot_name.c_str(), "graphviz/tree_graph.png", true);
    }
#endif
}

