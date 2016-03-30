#pragma once
#include "precomp.h"

#include "common/common.h"
#include "common/core.hpp"
#include "datalayer/archive/segmentation.h"

class OmSegment;
class OmSegmentsImpl;
class OmSegmentLists;
class SegmentDataWrapper;
class OmSegmentSelector;
class OmSegmentImpl;
class OmSegmentation;

namespace om {
namespace segment {
class Selection;
class Children;
class UserEdge;
}
namespace volume {
class MetadataManager;
}
}

class OmSegments {
 public:
  explicit OmSegments(OmSegmentation*);
  // set impl i.e. for testing
  OmSegments(OmSegmentation*, std::unique_ptr<OmSegmentsImpl>);
  ~OmSegments();

  void Flush();

  OmSegment* AddSegment();
  OmSegment* AddSegment(om::common::SegID value);
  OmSegment* GetSegment(const om::common::SegID) const;
  OmSegment* GetOrAddSegment(const om::common::SegID);

  bool IsSegmentValid(om::common::SegID seg);

  int GetNumSegments() const;
  void SetNumSegments(int);

  om::segment::Children& Children() const;
  om::segment::Selection& Selection() const;
  OmSegmentLists& SegmentLists() const;

  std::string GetName(om::common::SegID segID);
  void SetName(om::common::SegID segID, std::string name);

  std::string GetTags(om::common::SegID segID);
  void SetTags(om::common::SegID segID, std::string name);

  std::string GetNote(om::common::SegID segID);
  void SetNote(om::common::SegID segID, std::string note);

  boost::optional<std::string> IsEdgeSplittable(const om::segment::UserEdge& e);
  boost::optional<std::string> IsSegmentSplittable(OmSegment* child);
  boost::optional<std::string> IsSegmentCuttable(OmSegment* seg);

  OmSegment* FindRoot(OmSegment* segment);
  OmSegment* FindRoot(const om::common::SegID segID);
  om::common::SegID FindRootID(const om::common::SegID segID);
  om::common::SegID FindRootID(OmSegment* segment);

  std::pair<bool, om::segment::UserEdge> JoinEdge(
      const om::segment::UserEdge& e);
  om::segment::UserEdge SplitEdge(const om::segment::UserEdge& e);
  om::common::SegIDSet JoinTheseSegments(
      const om::common::SegIDSet& segmentList);
  om::common::SegIDSet UnJoinTheseSegments(
      const om::common::SegIDSet& segmentList);

  std::vector<om::segment::UserEdge> CutSegment(OmSegment* seg);
  bool JoinEdges(const std::vector<om::segment::UserEdge>& edges);
  std::vector<om::segment::UserEdge> Shatter(OmSegment* seg);

  void AddSegments_BreadthFirstSearch(OmSegmentSelector* sel, om::common::SegID segID);
  void AddSegments_BFS_DynamicThreshold(OmSegmentSelector* sel, om::common::SegID segID);
  void Trim(OmSegmentSelector* sel, om::common::SegID segID);

  uint32_t getPageSize();

  uint32_t maxValue() const;

  void refreshTree();

  bool AreAnySegmentsInValidList(const om::common::SegIDSet& ids);

 private:
  zi::mutex mutex_;

  om::volume::MetadataManager& meta_;
  std::unique_ptr<OmSegmentsImpl> impl_;

  friend class YAML::convert<OmSegments>;
  friend QDataStream& operator<<(QDataStream& out, const OmSegments& sc);
  friend QDataStream& operator>>(QDataStream& in, OmSegments& sc);
};
