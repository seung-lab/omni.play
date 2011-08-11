#pragma once

#include "yaml-cpp/yaml.h"

class OmSegmentationManager;
class OmSegmentation;
class OmSegments;
class OmSegmentsImpl;
class OmSegmentEdge;
class OmGroups;
class OmGroup;

namespace om {
namespace data {
namespace archive {

YAML::Emitter &operator<<(YAML::Emitter & out, const OmSegmentationManager& m);
void operator>>(const YAML::Node& in, OmSegmentationManager& m);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmSegmentation& seg);
void operator>>(const YAML::Node& in, OmSegmentation& seg);
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


}; // namespace archive
}; // namespace data
}; // namespace om