#ifndef OM_STRING_HPP
#define OM_STRING_HPP

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

};
};

#endif
