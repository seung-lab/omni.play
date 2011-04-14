#ifndef OM_JSON_IMPL_HPP
#define OM_JSON_IMPL_HPP

#include "common/omException.h"
#include "json_spirit.h"

#include <boost/utility.hpp>

// uses http://www.codeproject.com/KB/recipes/JSON_Spirit.aspx

namespace om {
namespace json {

typedef json_spirit::mValue reader_t;
typedef json_spirit::mObject writer_t;
typedef json_spirit::mObject jsonMap_t;

inline void read(const std::string& str, reader_t& value){
    json_spirit::read(str, value);
}

inline std::string write(const writer_t& writer)
{
    // return json_spirit::write(writer); // don't pretty-print
    return json_spirit::write_formatted(writer);
}

json_spirit::Value_type Type(const reader_t& value) {
    return value.type();
}

bool HasKey(const reader_t& value, const std::string& key)
{
    if(json_spirit::obj_type != value.type()){
        return false;
    }

    jsonMap_t map = value.get_obj();
    return map.count(key);
}

template <typename T>
boost::optional<T> GetValue(const reader_t& value, const std::string& key)
{
    if(json_spirit::obj_type != value.type()){
        return boost::optional<T>();
    }

    const jsonMap_t& map = value.get_obj();

    try {
        const reader_t& val = map.at(key);

        return boost::optional<T>(val.get_value<T>());
    } catch(std::exception& e){
        std::cout << "JSON parsing error: " << e.what() << "\n";
        return boost::optional<T>();
    }
}

template <typename T>
boost::optional<std::vector<T> > GetVector(const reader_t& value, const std::string& key)
{
    boost::optional<std::vector<T> > ret;

    if(json_spirit::obj_type != value.type()){
        return ret;
    }

    const jsonMap_t& map = value.get_obj();

    try {
        const reader_t& val = map.at(key);
        json_spirit::mArray array = val.get_array();
        std::vector<T> vec;
        FOR_EACH(iter, array){
            vec.push_back(iter->get_value<T>());
        }

        return boost::optional<std::vector<T> >(vec);

    } catch(std::exception& e){
        std::cout << "JSON parsing error: " << e.what() << "\n";
        return boost::optional<std::vector<T> >();
    }
}

void DumpValues(const reader_t& value)
{
   if(json_spirit::obj_type != value.type()){
       return;
    }

    jsonMap_t map = value.get_obj();
    FOR_EACH(iter, map){
        std::cout << iter->second.type() << "\n";
    }
}

std::string TypeStr(const reader_t& value)
{
    switch(Type(value)){
    case json_spirit::obj_type:
        return "obj_type";
    case json_spirit::array_type:
        return "array_type";
    case json_spirit::str_type:
        return "str_type";
    case json_spirit::bool_type:
        return "bool_type";
    case json_spirit::int_type:
        return "int_type";
    case json_spirit::real_type:
        return "real_type";
    case json_spirit::null_type:
        return "null_type";
    default:
        throw OmArgException("unknown json type");
    }
}

std::string Write(/*type....*/)
{
    writer_t out;

    // push_back onto out

    return write(out);
}

} // namespace json
} // namespace om

#endif
