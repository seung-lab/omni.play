#pragma once

#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "datalayer/archive/segmentation.h"

class OmSegmentEdge;
class OmUserEdges;
class OmSegmentChildren;
class OmSegmentsStore;

class OmSegmentsImpl : public OmSegmentsImplLowLevel {
 public:
  OmSegmentsImpl(OmSegmentation*, OmSegmentsStore*);
  virtual ~OmSegmentsImpl();

  void Flush();

  OmSegment* AddSegment();
  OmSegment* AddSegment(om::common::SegID value);
  OmSegment* GetOrAddSegment(const om::common::SegID val);

  OmSegmentChildren* Children();

  std::pair<bool, OmSegmentEdge> JoinFromUserAction(const OmSegmentEdge& e);
  OmSegmentEdge SplitEdgeUserAction(const OmSegmentEdge& e);
  om::common::SegIDSet JoinTheseSegments(
      const om::common::SegIDSet& segmentList);
  om::common::SegIDSet UnJoinTheseSegments(
      const om::common::SegIDSet& segmentList);
  boost::optional<std::string> IsEdgeSplittable(const OmSegmentEdge& e);
  boost::optional<std::string> IsSegmentSplittable(OmSegment* child);
  boost::optional<std::string> IsSegmentCuttable(OmSegment* seg);

  void refreshTree();

  bool AreAnySegmentsInValidList(const om::common::SegIDSet& ids);

  std::vector<OmSegmentEdge> CutSegment(OmSegment* seg);
  bool JoinEdges(const std::vector<OmSegmentEdge>& edges);
  std::vector<OmSegmentEdge> Shatter(OmSegment* seg);

 private:
  OmUserEdges* userEdges_;

  OmSegmentEdge splitChildFromParentNoTest(OmSegment* child);

  std::pair<bool, OmSegmentEdge> JoinEdgeFromUser(const OmSegmentEdge& e);
  std::pair<bool, OmSegmentEdge> JoinFromUserAction(const om::common::ID,
                                                    const om::common::ID);

  void rerootSegmentLists();
  void setGlobalThreshold(OmMST* mst);
  void resetGlobalThreshold(OmMST* mst);

  friend class OmSegmentColorizer;
  friend YAMLold::Emitter& YAMLold::operator<<(YAMLold::Emitter&,
                                               const OmSegmentsImpl&);
  friend void YAMLold::operator>>(const YAMLold::Node&, OmSegmentsImpl&);
  friend QDataStream& operator<<(QDataStream&, const OmSegmentsImpl&);
  friend QDataStream& operator>>(QDataStream&, OmSegmentsImpl&);
};
