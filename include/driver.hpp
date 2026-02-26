#pragma once

#include <cstdint>
#include <iostream>

namespace Driver
{

#ifdef SET_MODE_ENABLED
constexpr bool kVerifyWithSet = true;
#else
constexpr bool kVerifyWithSet = false;
#endif

static bool read_next(std::istream &in,
                      char         &mode,
                      int64_t      &a,
                      int64_t      &b)
{
    if (!(in >> mode))
        return false;

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

    std::cerr << "ERROR: unknown mode '" << mode << "'\n";
    return false;
}


template <typename TreeT, typename PolicyT>
int run(TreeT &tree, PolicyT &policy)
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    char    mode = 0;
    int64_t a    = 0;
    int64_t b    = 0;

    while (read_next(std::cin, mode, a, b))
    {
        if (mode == 'k')
        {
            policy.insert(tree, a);
            continue;
        }

        const auto ans = policy.query(tree, a, b);
        policy.handle_answer(a, b, ans);
    }

    policy.finalize();

    return 0;
}

} // namespace Driver