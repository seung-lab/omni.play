#pragma once

#include "common/common.h"
#include <QDataStream>

template<typename T>
inline QDataStream& operator<<(QDataStream& out, const Vector3<T>& v)
{
    out << v.array[0];
    out << v.array[1];
    out << v.array[2];
    return out;
}

template<typename T>
inline QDataStream& operator>>(QDataStream& in, Vector3<T>& v)
{
    in >> v.array[0];
    in >> v.array[1];
    in >> v.array[2];
    return in;
}

