#pragma once

#include "common/common.h"
#include "zi/omUtility.h"

#include <QDataStream>

template <typename T>
QDataStream &operator<<(QDataStream & out, const std::vector<T>& vec )
{
    out << (quint32)vec.size();

    //TODO: make faster
    FOR_EACH(iter, vec){
        out << *iter;
    }

    return out;
}

template <typename T>
QDataStream &operator>>(QDataStream & in, std::vector<T>& vec)
{
    quint32 size;
    in >> size;

    vec.resize(size);

    for(uint32_t i = 0; i < size; ++i ){
        in >> vec[i];
    }

    return in;
}

