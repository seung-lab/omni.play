#include "utility/yaml/omBaseTypes.hpp"
#include "datalayer/archive/segmentation.h"
#include "datalayer/archive/mipVolume.hpp"
#include "coordinates/yaml.hpp"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "segment/selection.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "project/details/omSegmentationManager.h"
#include "utility/yaml/genericManager.hpp"
#include "datalayer/archive/dummy.hpp"

namespace YAML {

Node convert<OmSegmentationManager>::encode(const OmSegmentationManager& m) {
  return genericManager::Save(m.manager_);
}
bool convert<OmSegmentationManager>::decode(const Node& node,
                                            OmSegmentationManager& m) {
  return genericManager::Load(node, m.manager_);
}

Node convert<OmSegmentation>::encode(const OmSegmentation& seg) {
  Node n;
  om::data::archive::mipVolume<const OmSegmentation> volArchive(seg);
  volArchive.Store(n);

  n["Segments"] = *seg.segments_;
  n["Num Edges"] = seg.mst_->size();
  return n;
}

bool convert<OmSegmentation>::decode(const Node& node, OmSegmentation& seg) {
  om::data::archive::mipVolume<OmSegmentation> volArchive(seg);
  volArchive.Load(node);

  YAML::convert<OmSegments>::decode(node["Segments"], *seg.segments_);

  seg.LoadVolDataIfFoldersExist();

  seg.segments_->refreshTree();
  return true;
}

Node convert<OmSegments>::encode(const OmSegments& sc) {
  Node n = convert<OmSegmentsImpl>::encode(*sc.impl_);
  n["Num Segments"] = sc.GetNumSegments();
  n["Max Value"] = sc.maxValue();
  return n;
}

bool convert<OmSegments>::decode(const Node& node, OmSegments& sc) {
  return YAML::convert<OmSegmentsImpl>::decode(node, *sc.impl_);
}

Node convert<OmSegmentsImpl>::encode(const OmSegmentsImpl& sc) {
  Node n;
  n["Enabled Segments"] = std::set<om::common::SegID>();
  n["Selected Segments"] = sc.Selection().selected_;

  n["Segment Custom Names"] = sc.segmentCustomNames_;
  n["Segment Notes"] = sc.segmentNotes_;
  return n;
}

bool convert<OmSegmentsImpl>::decode(const Node& node, OmSegmentsImpl& sc) {
  sc.segmentCustomNames_ =
      node["Segment Custom Names"]
          .as<std::unordered_map<om::common::ID, std::string>>();
  sc.segmentNotes_ = node["Segment Notes"]
                         .as<std::unordered_map<om::common::ID, std::string>>();
  return true;
}

}  // namespace YAML
