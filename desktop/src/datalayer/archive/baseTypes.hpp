#pragma once

#include "common/common.h"
#include "yaml-cpp/yaml.h"

#include <QHash>

class QString;

namespace YAML {

inline Emitter &operator<<(Emitter& out, const QString& s) {
    return out << s.toStdString();
}

inline void operator>>(const Node& in, QString& s) {
    std::string str;
    in >> str;

    if(str == "~") // NULL Value from YAML
        str = "";

    s = QString::fromStdString(str);
}

template<class K, class T>
Emitter &operator<<(Emitter& out, const QHash<K, T>& p)
{
    out << BeginMap;
    FOR_EACH(it, p)
    {
        out << Key << it.key();
        out << Value << it.value();
    }
    out << EndMap;

    return out;
}

template<class Key, class T>
void operator>>(const Node& in, QHash<Key, T>& p)
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

} // namespace YAML