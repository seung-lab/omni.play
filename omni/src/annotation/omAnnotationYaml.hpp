#pragma once

#include "annotation/omAnnotation.hpp"
#include "utility/yaml/omYaml.hpp"

void operator>>(const YAML::Node& node, OmColor& c)
{
    node[0] >> c.red;
    node[1] >> c.green;
    node[2] >> c.blue;
}

void operator>> (const YAML::Node& node, om::annotation::data& data)
{
    node["id"] >> data.id;
    node["x"] >> data.x;
    node["y"] >> data.y;
    node["z"] >> data.z;
    node["comment"] >> data.comment;
    node["color"] >> data.color;
}

namespace YAML
{

YAML::Emitter& operator<<(YAML::Emitter& out, const OmColor& c)
{
    out << YAML::BeginSeq;
    out << c.red;
    out << c.green;
    out << c.blue;
    out << YAML::EndSeq;

    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const om::annotation::data& data)
{
    out << YAML::Flow;

    out << YAML::BeginMap;

    out << YAML::Key << "id";
    out << YAML::Value << data.id;

    out << YAML::Key << "x";
    out << YAML::Value << data.x;
    out << YAML::Key << "y";
    out << YAML::Value << data.y;
    out << YAML::Key << "z";
    out << YAML::Value << data.z;

    out << YAML::Key << "comment";
    out << YAML::Value << data.comment;

    out << YAML::Key << "color";
    out << YAML::Value << data.color;

    out << YAML::EndMap;

    return out;
}

} // namespace YAML
