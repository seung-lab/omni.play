#pragma once

#include "volume/omVolumeTypes.hpp"
#include "segment/omSegmentPointers.h"
#include "datalayer/archive/segmentation.h"

#include <zi/mutex.hpp>

class OmSegment;
class OmSegmentChildren;
class OmSegmentation;
class OmSegmentsImpl;
class OmSegmentsStore;
class OmSegmentDataWrapper;

namespace om {
namespace segment {
class Selection;
}
}

class OmSegments {
 public:
  OmSegments(OmSegmentation* segmentation);
  ~OmSegments();

  void StartCaches();
  void Flush();

  OmSegment* AddSegment();
  OmSegment* AddSegment(om::common::SegID value);
  OmSegment* GetSegment(const om::common::SegID);
  OmSegment* GetSegmentUnsafe(const om::common::SegID);
  OmSegment* GetOrAddSegment(const om::common::SegID);

  bool IsSegmentValid(om::common::SegID seg);

  om::common::SegID GetNumSegments();
  om::common::SegID GetNumTopSegments();

  OmSegmentChildren* Children();

  bool isSegmentEnabled(om::common::SegID segID);
  void setSegmentEnabled(om::common::SegID segID, bool isEnabled);
  om::common::SegIDSet GetEnabledSegmentIDs();
  bool AreSegmentsEnabled();

  bool IsSegmentSelected(om::common::SegID segID);
  bool IsSegmentSelected(OmSegment* seg);
  void setSegmentSelected(om::common::SegID segID, const bool, const bool);
  const om::common::SegIDSet GetSelectedSegmentIDs();
  uint32_t NumberOfSelectedSegments();
  bool AreSegmentsSelected();
  void UpdateSegmentSelection(const om::common::SegIDSet& idsToSelect,
                              const bool);
  void AddToSegmentSelection(const om::common::SegIDSet& idsToSelect);
  void RemoveFromSegmentSelection(const om::common::SegIDSet& idsToSelect);

  QString getSegmentName(om::common::SegID segID);
  void setSegmentName(om::common::SegID segID, QString name);

  QString getSegmentNote(om::common::SegID segID);
  void setSegmentNote(om::common::SegID segID, QString note);

  om::common::SegID GetSegmentationID();

  boost::optional<std::string> IsEdgeSplittable(const OmSegmentEdge& e);
  boost::optional<std::string> IsSegmentSplittable(OmSegment* child);
  boost::optional<std::string> IsSegmentCuttable(OmSegment* seg);

  OmSegment* findRoot(OmSegment* segment);
  OmSegment* FindRoot(OmSegment* segment);
  OmSegment* findRoot(const om::common::SegID segID);
  om::common::SegID findRootID(const om::common::SegID segID);
  om::common::SegID findRootID(OmSegment* segment);
  om::common::SegID findRootIDnoCache(const om::common::SegID segID);

  std::pair<bool, OmSegmentEdge> JoinEdge(const OmSegmentEdge& e);
  OmSegmentEdge SplitEdge(const OmSegmentEdge& e);
  om::common::SegIDSet JoinTheseSegments(
      const om::common::SegIDSet& segmentList);
  om::common::SegIDSet UnJoinTheseSegments(
      const om::common::SegIDSet& segmentList);

  std::vector<OmSegmentEdge> CutSegment(OmSegment* seg);
  bool JoinEdges(const std::vector<OmSegmentEdge>& edges);
  std::vector<OmSegmentEdge> Shatter(OmSegment* seg);

  uint32_t getPageSize();

  uint32_t getMaxValue();

  void refreshTree();

  bool AreAnySegmentsInValidList(const om::common::SegIDSet& ids);

  uint64_t MSTfreshness() const;

  inline const OmSegmentation* getSegmentation() const { return segmentation_; }

  om::segment::Selection& Selection() const;

 private:
  zi::mutex mutex_;

  OmSegmentation* const segmentation_;

  std::unique_ptr<OmSegmentsStore> store_;
  std::unique_ptr<OmSegmentsImpl> impl_;
  std::unique_ptr<om::segment::Selection> selection_;

  friend class OmSegmentColorizer;
  friend class SegmentTests;

  friend YAMLold::Emitter& YAMLold::operator<<(YAMLold::Emitter& out,
                                               const OmSegments& sc);
  friend void YAMLold::operator>>(const YAMLold::Node& in, OmSegments& sc);
  friend QDataStream& operator<<(QDataStream& out, const OmSegments& sc);
  friend QDataStream& operator>>(QDataStream& in, OmSegments& sc);
};
