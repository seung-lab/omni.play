#pragma once

#include "annotation/annotation.h"
#include "yaml-cpp/yaml.h"
#include "utility/yaml/baseTypes.hpp"
#include "utility/yaml/omYaml.hpp"

namespace YAML {

void operator>> (const Node& node, om::annotation::data& data)
{
    node["coord"] >> data.coord;
    node["comment"] >> data.comment;
    if(data.comment == "~")
        data.comment = "";
    node["color"] >> data.color;
	om::yaml::util::OptionalRead(node, "size", data.size, 0.03);
}

Emitter& operator<<(Emitter& out, const om::annotation::data& data)
{
    out << Flow;

    out << BeginMap;
    out << Key << "coord" << Value << data.coord;
    out << Key << "comment" << Value << data.comment;
    out << Key << "color" << Value << data.color;
    out << Key << "size" << Value << data.size;
    out << EndMap;

    return out;
}


} // namespace YAML