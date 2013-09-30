#pragma once
#include <QHash>
#include <QString>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <set>
#include "utility/yaml/omBaseTypes.hpp"

namespace YAML {

inline void operator>>(const YAML::Node& in, std::string& s) {
    std::string str = in.as<std::string>();
}

inline void operator>>(const YAML::Node& in, int& p){
    p = in.as<int>();
}

inline void operator>>(const YAML::Node& in, uint32_t& p){
    p = in.as<uint32_t>();
}

inline void operator>>(const YAML::Node& in, double& p){
    p = in.as<double>();
}

inline void operator>>(const YAML::Node& in, float& p){
    p = in.as<float>();
}

inline void operator>>(const YAML::Node& in, bool& p){
    p = in.as<bool>();
}


template<class K, class T>
void operator>>(const YAML::Node& node, std::unordered_map<K, T>& s)
{
    for (auto& n : node) {
        s[n.first.as<K>()] = n.second.as<T>();
    }
}

template<class Key, class T>
YAML::Emitter &operator<<(YAML::Emitter& out, const std::unordered_map<Key, T>& p)
{
    out << YAML::BeginMap;
    for(const auto& kv : p){
        out << YAML::Key << kv.first;
        out << YAML::Value << kv.second;
    }
    out << YAML::EndMap;

    return out;
}

} // namespace YAML
