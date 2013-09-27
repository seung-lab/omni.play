#pragma once

#include "annotation/annotation.h"
#include "yaml-cpp/yaml.h"
#include "utility/yaml/omBaseTypes.hpp"
#include "coordinates/yaml.h"

namespace YAML {

Emitter& operator<<(Emitter& out, const om::annotation::data& data)
{
    out << Flow;

    out << BeginMap;
    out << Key << "coord" << Value << data.coord.toGlobalCoord();
    out << Key << "comment" << Value << data.comment;
    out << Key << "color" << Value << data.color;
    out << Key << "size" << Value << data.size;
    out << EndMap;

    return out;
}

} // namespace YAML
