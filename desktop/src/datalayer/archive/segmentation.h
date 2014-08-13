#pragma once
#include "precomp.h"

class OmSegmentationManager;
class OmSegmentation;
class OmSegments;
class OmSegmentsImpl;
class DummyGroups;
class DummyGroup;

namespace YAML {

template <>
struct convert<OmSegmentationManager> {
  static Node encode(const OmSegmentationManager& fm);
  static bool decode(const Node& node, OmSegmentationManager& fm);
};

template <>
struct convert<OmSegmentation> {
  static Node encode(const OmSegmentation& fm);
  static bool decode(const Node& node, OmSegmentation& fm);
};

template <>
struct convert<OmSegments> {
  static Node encode(const OmSegments& fm);
  static bool decode(const Node& node, OmSegments& fm);
};

template <>
struct convert<OmSegmentsImpl> {
  static Node encode(const OmSegmentsImpl& fm);
  static bool decode(const Node& node, OmSegmentsImpl& fm);
};

}  // namespace YAML
