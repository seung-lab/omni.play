#pragma once

#include "common.h"
#include "yaml-cpp/yaml.h"

class QString;

namespace YAML {

inline Emitter& operator<<(Emitter& out, const om::common::color& c)
{
    out << Flow << BeginSeq;
    out << c.red;
    out << c.green;
    out << c.blue;
    out << EndSeq;
    
    return out;
}

inline void operator>>(const Node& node, om::common::color& c)
{
    // workaround: Reading into uint8_ts appears to be bugged out.
    int temp;
    node[0] >> temp;
    c.red = temp;
    node[1] >> temp;
    c.green = temp;
    node[2] >> temp;
    c.blue = temp;
}

template<class T>
Emitter &operator<<(Emitter& out, const Vector3<T>& p)
{
    out << Flow;
    out << BeginSeq << p.x << p.y << p.z << EndSeq;
    return out;
}

template<class T>
void operator>>(const Node& in, Vector3<T>& p)
{
    in[0] >> p.x;
    in[1] >> p.y; 
    in[2] >> p.z;
}

template<class T>
Emitter &operator<<(Emitter& out, const Matrix4<T>& p)
{
    out << Flow << BeginSeq;
    for(int i = 0; i < 16; i++)
    {
        out << p.array[i];
    }
    out << EndSeq;
    return out;
}

template<class T>
void operator>>(const Node& in, Matrix4<T>& p)
{
    T f[16];
    for(int i = 0; i < 16; i++)
    {
        in[i] >> f[i];
    }
    p.set(f);
}

template<class T>
Emitter &operator<<(Emitter& out, const AxisAlignedBoundingBox<T>& b)
{
    out << BeginMap;
    out << Key << "min" << Value << b.getMin();
    out << Key << "max" << Value << b.getMax();
    out << EndMap;
    return out;
}

template<class T>
void operator>>(const Node& in, AxisAlignedBoundingBox<T>& b)
{
    Vector3<T> min, max;
    in["min"] >> min;
    in["max"] >> max;
    b = AxisAlignedBoundingBox<T>(min, max);
}

template<class T>
Emitter &operator<<(Emitter& out, const boost::unordered_set<T>& s)
{
    out << Flow << BeginSeq;
    FOR_EACH(it, s)
    {
        out << *it;
    }
    out <<  EndSeq;
    return out;
}

template<class T>
void operator>>(const Node& in, boost::unordered_set<T>& s)
{
    FOR_EACH(it, in)
    {
        T item;
        *it >> item;
        s.insert(item);
    }
}

template<class T>
Emitter &operator<<(Emitter& out, const std::set<T>& s)
{
    out << Flow << BeginSeq;
    FOR_EACH(it, s)
    {
        out << *it;
    }
    out <<  EndSeq;
    return out;
}

template<class T>
void operator>>(const Node& in, std::set<T>& s)
{
    FOR_EACH(it, in)
    {
        T item;
        *it >> item;
        s.insert(item);
    }
}

} // namespace YAML