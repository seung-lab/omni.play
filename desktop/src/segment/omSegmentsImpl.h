#pragma once
#include "precomp.h"

#include <unordered_map>

#include "segment/omSegment.h"
#include "segment/lowLevel/graph.h"
#include "segment/dataSources.hpp"
#include "segment/lists/omSegmentLists.h"

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

  // barebones constructor for mocking
  OmSegmentsImpl(OmSegmentation* segmentation);

  void Flush();

  OmSegment* AddSegment();
  OmSegment* AddSegment(om::common::SegID value);
  OmSegment* GetOrAddSegment(const om::common::SegID val);

  om::segment::Children& Children() const;

  std::pair<bool, om::segment::UserEdge> JoinFromUserAction(
      const om::segment::UserEdge& e);
  om::segment::UserEdge SplitEdgeUserAction(const om::segment::UserEdge& e);
  om::segment::UserEdge SplitEdgeMinCutUserAction(const om::segment::UserEdge& e);
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
  virtual om::common::SegID FindRootID(const om::common::SegID segID) const {
    if (!segID) {
      return 0;
    }

    return graph_->Root(segID);
  }

  virtual om::common::SegID FindRootID(const OmSegment* segment) const {
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

  virtual const std::unordered_map<om::common::SegID,
            std::vector <om::segment::Edge*>>& GetAdjacencyMap() const;
  virtual OmSegment* FindRoot(const om::common::SegID segID) const;
  virtual OmSegment* FindRoot(const OmSegment* segment) const;

  bool IsSegmentValid(om::common::SegID seg);
  virtual OmSegment* GetSegment(const om::common::SegID value) const;

  std::string GetName(om::common::SegID segID);
  void SetName(om::common::SegID segID, std::string name);

  std::string GetTags(om::common::SegID segID);
  void SetTags(om::common::SegID segID, std::string name);

  std::string GetNote(om::common::SegID segID);
  void SetNote(om::common::SegID segID, std::string note);

  void turnBatchModeOn(const bool batchMode);

 private:
  om::segment::EdgeVector& mst_;
  om::segment::UserEdgeVector& userEdges_;
  OmValidGroupNum& valid_;
  om::volume::MetadataManager& meta_;

  std::unique_ptr<om::segment::Store> store_;
  std::unique_ptr<om::segment::Graph> graph_;
  std::unique_ptr<OmSegmentLists> segmentLists_;
  std::unique_ptr<om::segment::Selection> selection_;
  std::unique_ptr<om::segment::GraphThresholder> thresholder_;

  std::unordered_map<om::common::ID, std::string> segmentCustomNames_;
  std::unordered_map<om::common::ID, std::string> segmentCustomTags_;
  std::unordered_map<om::common::ID, std::string> segmentNotes_;
  std::unordered_map<om::common::SegID, std::vector <om::segment::Edge*>> adjacencyMap_;

  om::segment::UserEdge splitChildFromParentNoTest(OmSegment* child);

  std::pair<bool, om::segment::UserEdge> JoinEdgeFromUser(
      const om::segment::UserEdge& e);
  std::pair<bool, om::segment::UserEdge> JoinFromUserAction(
      const om::common::ID, const om::common::ID);

  void setGlobalThreshold();
  void resetGlobalThreshold();
  void resetSizeThreshold();
  void SortAdjacencyMap();
  void populateAdjacencyMap();

  friend class YAML::convert<OmSegmentsImpl>;
};
