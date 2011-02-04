#ifndef OM_SET_OPP_HPP
#define OM_SET_OPP_HPP

namespace om{
namespace sets{

template <typename T>
inline bool SetAContainsB(const std::set<T>& a, const std::set<T> b)
{
    std::set<T> diff;

    zi::set_difference(b.begin(), b.end(), a.begin(), a.end(),
                       std::inserter(diff, diff.begin()));

    return 0 == diff.size();
}

template <typename T>
inline bool SetsAreDisjoint(const std::set<T>& a, const std::set<T> b)
{
    std::set<T> inter;

    zi::set_intersection(b.begin(), b.end(), a.begin(), a.end(),
                         std::inserter(inter, inter.begin()));

    return 0 == inter.size();
}

}
}

#endif
