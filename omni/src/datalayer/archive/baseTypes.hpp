#pragma once

#include "common/om.hpp"
#include "common/omCommon.h"
#include "yaml-cpp/yaml.h"

#include <QHash>

class QString;

namespace om {
namespace data {
namespace archive {

inline YAML::Emitter &operator<<(YAML::Emitter& out, const QString& s) {
    return out << s.toStdString();
}

inline void operator>>(const YAML::Node& in, QString& s) {
    std::string str;
    in >> str;
    
    if(str == "~") // NULL Value from YAML
        str = "";
    
    s = QString::fromStdString(str);   
}

template<class T>
YAML::Emitter &operator<<(YAML::Emitter& out, const Vector3<T>& p)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << p.x << p.y << p.z << YAML::EndSeq;
    return out;
}

template<class T>
void operator>>(const YAML::Node& in, Vector3<T>& p)
{
    in[0] >> p.x;
    in[1] >> p.y; 
    in[2] >> p.z;
}

template<class T>
YAML::Emitter &operator<<(YAML::Emitter& out, const Matrix4<T>& p)
{
    out << YAML::Flow << YAML::BeginSeq;
    for(int i = 0; i < 16; i++)
    {
        out << p.array[i];
    }
    out << YAML::EndSeq;
    return out;
}

template<class T>
void operator>>(const YAML::Node& in, Matrix4<T>& p)
{
    T f[16];
    for(int i = 0; i < 16; i++)
    {
        in[i] >> f[i];
    }
    p.set(f);
}

template<class T>
YAML::Emitter &operator<<(YAML::Emitter& out, const vmml::AxisAlignedBoundingBox<T>& b)
{
    out << YAML::BeginMap;
    out << YAML::Key << "min" << YAML::Value << b.getMin();
    out << YAML::Key << "max" << YAML::Value << b.getMax();
    out << YAML::EndMap;
    return out;
}

template<class T>
void operator>>(const YAML::Node& in, vmml::AxisAlignedBoundingBox<T>& b)
{
    Vector3<T> min, max;
    in["min"] >> min;
    in["max"] >> max;
    b = AxisAlignedBoundingBox<T>(min, max);
}

template<class T>
YAML::Emitter &operator<<(YAML::Emitter& out, const boost::unordered_set<T>& s)
{
    out << YAML::Flow << YAML::BeginSeq;
    FOR_EACH(it, s)
    {
        out << *it;
    }
    out << YAML:: EndSeq;
    return out;
}

template<class T>
void operator>>(const YAML::Node& in, boost::unordered_set<T>& s)
{
    FOR_EACH(it, in)
    {
        T item;
        *it >> item;
        s.insert(item);
    }
}

template<class T>
YAML::Emitter &operator<<(YAML::Emitter& out, const std::set<T>& s)
{
    out << YAML::Flow << YAML::BeginSeq;
    FOR_EACH(it, s)
    {
        out << *it;
    }
    out << YAML:: EndSeq;
    return out;
}

template<class T>
void operator>>(const YAML::Node& in, std::set<T>& s)
{
    FOR_EACH(it, in)
    {
        T item;
        *it >> item;
        s.insert(item);
    }
}

template<class Key, class T>
YAML::Emitter &operator<<(YAML::Emitter& out, const QHash<Key, T>& p)
{
    out << YAML::BeginMap;
    FOR_EACH(it, p)
    {
        out << YAML::Key << it.key();
        out << YAML::Value << it.value();
    }
    out << YAML::EndMap;
    
    return out;
}

template<class Key, class T>
void operator>>(const YAML::Node& in, QHash<Key, T>& p)
{
    FOR_EACH(it, in) 
    {
        Key key;
        T value;
        it.first() >> key;
        it.second() >> value;
        p.insert(key, value);
    }
}



}; // namespace archive
}; // namespace data
}; // namespace om