#pragma once
#include "precomp.h"

class OmSegmentationManager;
class OmSegmentation;
class OmSegments;
class OmSegmentsImpl;
class DummyGroups;
class DummyGroup;

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

YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const DummyGroups& g);
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const DummyGroup& g);

}  // namespace YAMLold
