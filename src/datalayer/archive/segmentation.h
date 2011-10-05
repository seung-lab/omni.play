#pragma once

#include "yaml-cpp/yaml.h"

class segmentationManager;
class segmentation;
class segments;
class segmentsImpl;
class segmentEdge;
class OmGroups;
class OmGroup;

namespace YAML {

YAML::Emitter &operator<<(YAML::Emitter & out, const segmentationManager& m);
void operator>>(const YAML::Node& in, segmentationManager& m);
YAML::Emitter &operator<<(YAML::Emitter& out, const segmentation& seg);
void operator>>(const YAML::Node& in, segmentation& seg);
YAML::Emitter &operator<<(YAML::Emitter& out, const segments& sc);
void operator>>(const YAML::Node& in, segments& sc);
YAML::Emitter &operator<<(YAML::Emitter& out, const segmentsImpl& sc);
void operator>>(const YAML::Node& in, segmentsImpl& sc);
YAML::Emitter &operator<<(YAML::Emitter& out, const segmentEdge& se);
void operator>>(const YAML::Node& in, segmentEdge& se);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmGroups& g);
void operator>>(const YAML::Node& in, OmGroups& g);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmGroup& g);
void operator>>(const YAML::Node& in, OmGroup& g);


} // namespace YAML