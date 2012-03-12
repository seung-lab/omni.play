#pragma once

#include "yaml-cpp/yaml.h"

namespace om {
namespace volume { class segmentation; }
namespace proj { class segmentationManager; }
namespace segment {
class segments;
class segmentsImpl;
class edge;
}
}

namespace YAML {

YAML::Emitter &operator<<(YAML::Emitter & out, const om::proj::segmentationManager& m);
void operator>>(const YAML::Node& in, om::proj::segmentationManager& m);
YAML::Emitter &operator<<(YAML::Emitter& out, const om::volume::segmentation& seg);
void operator>>(const YAML::Node& in, om::volume::segmentation& seg);
YAML::Emitter &operator<<(YAML::Emitter& out, const om::segment::segments& sc);
void operator>>(const YAML::Node& in, om::segment::segments& sc);
YAML::Emitter &operator<<(YAML::Emitter& out, const om::segment::segmentsImpl& sc);
void operator>>(const YAML::Node& in, om::segment::segmentsImpl& sc);
YAML::Emitter &operator<<(YAML::Emitter& out, const om::segment::edge& se);
void operator>>(const YAML::Node& in, om::segment::edge& se);

} // namespace YAML
