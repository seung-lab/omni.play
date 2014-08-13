#pragma once
#include "precomp.h"

#include "annotation/annotation.h"

#include "utility/yaml/omBaseTypes.hpp"
#include "coordinates/yaml.hpp"

namespace YAML {
template <>
struct convert<om::annotation::data> {
  static Node encode(const om::annotation::data& data) {
    Node n;
    n["coord"] = data.coord.ToGlobal();
    n["comment"] = data.comment;
    n["color"] = data.color;
    n["size"] = data.size;
    return n;
  }
};
}  // namespace YAML
