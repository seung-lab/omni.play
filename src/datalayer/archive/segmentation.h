#pragma once

#include "yaml-cpp/yaml.h"

class segmentationManager;
class segmentation;
class OmSegments;
class OmSegmentsImpl;
class OmSegmentEdge;
class OmGroups;
class OmGroup;

namespace YAML {

YAML::Emitter &operator<<(YAML::Emitter & out, const segmentationManager& m);
void operator>>(const YAML::Node& in, segmentationManager& m);
YAML::Emitter &operator<<(YAML::Emitter& out, const segmentation& seg);
void operator>>(const YAML::Node& in, segmentation& seg);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmSegments& sc);
void operator>>(const YAML::Node& in, OmSegments& sc);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmSegmentsImpl& sc);
void operator>>(const YAML::Node& in, OmSegmentsImpl& sc);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmSegmentEdge& se);
void operator>>(const YAML::Node& in, OmSegmentEdge& se);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmGroups& g);
void operator>>(const YAML::Node& in, OmGroups& g);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmGroup& g);
void operator>>(const YAML::Node& in, OmGroup& g);


} // namespace YAML