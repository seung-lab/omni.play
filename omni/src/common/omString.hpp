#ifndef OM_STRING_HPP
#define OM_STRING_HPP

#include <zi/for_each.hpp>

#include <boost/range/adaptor/transformed.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <string>

namespace om {
namespace string {

template <typename T>
static std::string num(const T& num){
    return boost::lexical_cast<std::string>(num);
}

template <typename T>
static T toNum(const std::string& str){
    return boost::lexical_cast<T>(str);
}

// from http://stackoverflow.com/questions/3804183/how-to-nicely-output-a-list-of-separated-strings
template <typename T>
static std::string join(const T& in, const std::string sep = ", ")
{
    return boost::algorithm::join(
        in  |
        boost::adaptors::transformed(boost::lexical_cast<std::string,int>),
        sep);
}

template <typename T>
inline static std::string humanizeNum(const T num, const char sep = ',')
{
    const std::string rawNumAsStr = om::string::num(num);

    size_t counter = 0;
    std::string ret;

    FOR_EACH_R(i, rawNumAsStr){
        ++counter;
        ret += *i;
        if( 0 == ( counter % 3 ) &&
            counter != rawNumAsStr.size() )
        {
            ret += sep;
        }
    }

    std::reverse(ret.begin(), ret.end());
    return ret;
}

template <typename T>
inline static std::string bytesToMB(const T num)
{
    static const int64_t bytes_per_mb = 1048576;

    const int64_t size = static_cast<int64_t>(num) / bytes_per_mb;
    return humanizeNum(size) + "MB";
}

};
};

#endif
