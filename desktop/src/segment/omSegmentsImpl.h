#pragma once
#include "precomp.h"

#include <unordered_map>

#include "segment/omSegment.h"
#include "segment/lowLevel/graph.h"
#include "segment/dataSources.hpp"

class OmSegmentSelection;
class SegmentationDataWrapper;

namespace om {
namespace segment {
class Store;
class Selection;
class GraphThresholder;
class Children;
struct UserEdge;
struct UserEdgeVector;
}
namespace volume {
class MetadataManager;
}
}

class OmSegmentsImpl {
 public:
  OmSegmentsImpl(om::segment::SegDataVector&, om::segment::SegListDataVector&,
                 om::segment::EdgeVector&, om::segment::UserEdgeVector&,
                 om::volume::MetadataManager&, OmValidGroupNum&,
                 SegmentationDataWrapper sdw);
  virtual ~OmSegmentsImpl();

  void Flush();

  OmSegment* AddSegment();
  OmSegment* AddSegment(om::common::SegID value);
  OmSegment* GetOrAddSegment(const om::common::SegID val);

  om::segment::Children& Children() const;

  std::pair<bool, om::segment::UserEdge> JoinFromUserAction(
      const om::segment::UserEdge& e);
  om::segment::UserEdge SplitEdgeUserAction(const om::segment::UserEdge& e);
  om::common::SegIDSet JoinTheseSegments(
      const om::common::SegIDSet& segmentList);
  om::common::SegIDSet UnJoinTheseSegments(
      const om::common::SegIDSet& segmentList);
  boost::optional<std::string> IsEdgeSplittable(const om::segment::UserEdge& e);
  boost::optional<std::string> IsSegmentSplittable(OmSegment* child);
  boost::optional<std::string> IsSegmentCuttable(OmSegment* seg);

  void refreshTree();

  bool AreAnySegmentsInValidList(const om::common::SegIDSet& ids);

  std::vector<om::segment::UserEdge> CutSegment(OmSegment* seg);
  bool JoinEdges(const std::vector<om::segment::UserEdge>& edges);
  std::vector<om::segment::UserEdge> Shatter(OmSegment* seg);

  // EX ImplLowLevel
  inline om::common::SegID FindRootID(const om::common::SegID segID) {
    if (!segID) {
      return 0;
    }

    return graph_->Root(segID);
  }

  inline om::common::SegID FindRootID(OmSegment* segment) {
    if (!segment) {
      return 0;
    }

    if (!segment->getParent()) {
      return segment->value();
    }

    return graph_->Root(segment->value());
  }

  om::segment::Selection& Selection() const { return *selection_; }
  OmSegmentLists& SegmentLists() const { return *segmentLists_; }

  OmSegment* FindRoot(const om::common::SegID segID);
  OmSegment* FindRoot(OmSegment* segment);

  bool IsSegmentValid(om::common::SegID seg);
  OmSegment* GetSegment(const om::common::SegID value) const;

  std::string GetName(om::common::SegID segID);
  void SetName(om::common::SegID segID, std::string name);

  std::string GetNote(om::common::SegID segID);
  void SetNote(om::common::SegID segID, std::string note);

  void turnBatchModeOn(const bool batchMode);

 private:
  om::segment::EdgeVector& mst_;
  om::segment::UserEdgeVector& userEdges_;
  om::volume::MetadataManager& meta_;

  std::unique_ptr<om::segment::Store> store_;
  std::unique_ptr<om::segment::Graph> graph_;
  std::unique_ptr<OmSegmentLists> segmentLists_;
  std::unique_ptr<om::segment::Selection> selection_;
  std::unique_ptr<om::segment::GraphThresholder> thresholder_;

  std::unordered_map<om::common::ID, std::string> segmentCustomNames_;
  std::unordered_map<om::common::ID, std::string> segmentNotes_;

  om::segment::UserEdge splitChildFromParentNoTest(OmSegment* child);

  std::pair<bool, om::segment::UserEdge> JoinEdgeFromUser(
      const om::segment::UserEdge& e);
  std::pair<bool, om::segment::UserEdge> JoinFromUserAction(
      const om::common::ID, const om::common::ID);

  void setGlobalThreshold();
  void resetGlobalThreshold();
};
