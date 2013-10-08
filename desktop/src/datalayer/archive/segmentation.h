#pragma once

#include "yaml-cpp-old/yaml.h"

class OmSegmentationManager;
class OmSegmentation;
class OmSegments;
class OmSegmentsImpl;
class OmSegmentEdge;
class OmGroups;
class OmGroup;

namespace YAMLold {

YAMLold::Emitter& operator<<(YAMLold::Emitter& out,
                             const OmSegmentationManager& m);
void operator>>(const YAMLold::Node& in, OmSegmentationManager& m);
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const OmSegmentation& seg);
void operator>>(const YAMLold::Node& in, OmSegmentation& seg);
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const OmSegments& sc);
void operator>>(const YAMLold::Node& in, OmSegments& sc);
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const OmSegmentsImpl& sc);
void operator>>(const YAMLold::Node& in, OmSegmentsImpl& sc);
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const OmSegmentEdge& se);
void operator>>(const YAMLold::Node& in, OmSegmentEdge& se);
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const OmGroups& g);
void operator>>(const YAMLold::Node& in, OmGroups& g);
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const OmGroup& g);
void operator>>(const YAMLold::Node& in, OmGroup& g);

}  // namespace YAMLold