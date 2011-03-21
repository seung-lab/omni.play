#ifndef OM_SET_OPP_HPP
#define OM_SET_OPP_HPP

#include <zi/for_each.hpp>
#include <zi/parallel/algorithm.hpp>

namespace om{
namespace set{

template <typename T>
inline bool SetAContainsB(const std::set<T>& a, const std::set<T>& b)
{
    std::set<T> diff;

    zi::set_difference(b.begin(), b.end(), a.begin(), a.end(),
                       std::inserter(diff, diff.begin()));

    return 0 == diff.size();
}

template <typename T>
inline bool SetsAreDisjoint(const std::set<T>& a, const std::set<T>& b)
{
    std::set<T> inter;

    zi::set_intersection(b.begin(), b.end(), a.begin(), a.end(),
                         std::inserter(inter, inter.begin()));

    return 0 == inter.size();
}

template <typename T>
inline void merge(const std::set<T>& a, const std::set<T>& b,
                  std::set<T>& c)
{
    zi::set_union(a.begin(), a.end(),
                  b.begin(), b.end(),
                  std::inserter(c, c.begin()));
}

template <typename T>
inline void mergeBintoA(std::set<T>& a, const std::set<T>& b)
{
    FOR_EACH(iter, b){
        a.insert(*iter);
    }
}

template <typename T>
inline void mergeBintoA(boost::unordered_set<T>& a,
                        const boost::unordered_set<T>& b)
{
    FOR_EACH(iter, b){
        a.insert(*iter);
    }
}

} //set
} //om

#endif
