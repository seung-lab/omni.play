#pragma once
#include "precomp.h"

#include <boost/optional.hpp>
#include <zi/mutex.hpp>

#include "common/core.hpp"
#include "datalayer/archive/segmentation.h"

class OmSegment;
class OmSegmentsImpl;
class OmSegmentLists;
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
  ~OmSegments();

  void Flush();

  OmSegment* AddSegment();
  OmSegment* AddSegment(om::common::SegID value);
  OmSegment* GetSegment(const om::common::SegID) const;
  OmSegment* GetOrAddSegment(const om::common::SegID);

  bool IsSegmentValid(om::common::SegID seg);

  int GetNumSegments();
  void SetNumSegments(int);

  om::segment::Children& Children() const;
  om::segment::Selection& Selection() const;
  OmSegmentLists& SegmentLists() const;

  std::string GetName(om::common::SegID segID);
  void SetName(om::common::SegID segID, std::string name);

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

  uint32_t getPageSize();

  uint32_t maxValue();

  void refreshTree();

  bool AreAnySegmentsInValidList(const om::common::SegIDSet& ids);

 private:
  zi::mutex mutex_;

  om::volume::MetadataManager& meta_;
  std::unique_ptr<OmSegmentsImpl> impl_;

  friend YAMLold::Emitter& YAMLold::operator<<(YAMLold::Emitter& out,
                                               const OmSegments& sc);
  friend void YAMLold::operator>>(const YAMLold::Node& in, OmSegments& sc);
  friend YAMLold::Emitter& YAMLold::operator<<(YAMLold::Emitter& out,
                                               const OmSegmentsImpl& sc);
  friend void YAMLold::operator>>(const YAMLold::Node& in, OmSegmentsImpl& sc);
};
