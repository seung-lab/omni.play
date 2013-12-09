#pragma once

#include "common/common.h"
#include "segment/lowLevel/store/omSegmentStore.hpp"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegment.h"
#include "utility/omLockedPODs.hpp"

#include <QHash>

class OmEnabledSegments;
class OmSegmentSelection;
class OmSegments;
class OmSegmentation;
class SegmentationDataWrapper;

class OmSegmentsImplLowLevel {
 public:
  OmSegmentsImplLowLevel(OmSegmentation*, OmSegmentsStore*);
  virtual ~OmSegmentsImplLowLevel();

  inline void RefreshGUIlists() { segmentGraph_.RefreshGUIlists(); }

  void growGraphIfNeeded(OmSegment* newSeg);

  inline om::common::SegID GetNumSegments() const { return mNumSegs; }

  void SetNumSegments(const uint32_t num) { mNumSegs = num; }

  inline OmSegment* FindRoot(OmSegment* segment) {
    if (!segment) {
      return 0;
    }

    if (!segment->getParent()) {
      return segment;
    }

    return store_->GetSegment(segmentGraph_.Root(segment->value()));
  }

  inline om::common::SegID FindRootID(OmSegment* segment) {
    if (!segment) {
      return 0;
    }

    if (!segment->getParent()) {
      return segment->value();
    }

    return segmentGraph_.Root(segment->value());
  }

  inline OmSegment* FindRoot(const om::common::SegID segID) {
    if (!segID) {
      return 0;
    }

    return store_->GetSegment(segmentGraph_.Root(segID));
  }

  inline om::common::SegID FindRootID(const om::common::SegID segID) {
    if (!segID) {
      return 0;
    }

    return segmentGraph_.Root(segID);
  }

  QString getSegmentName(om::common::SegID segID);
  void setSegmentName(om::common::SegID segID, QString name);

  QString getSegmentNote(om::common::SegID segID);
  void setSegmentNote(om::common::SegID segID, QString note);

  void turnBatchModeOn(const bool batchMode);

  inline uint32_t getMaxValue() const { return maxValue_.get(); }

  inline uint64_t MSTfreshness() const { return segmentGraph_.MSTfreshness(); }

  inline OmSegmentSelection& SegmentSelection() { return *segmentSelection_; }

  inline OmEnabledSegments& EnabledSegments() { return *enabledSegments_; }

  SegmentationDataWrapper GetSDW() const;

  OmSegmentsStore* SegmentStore() { return store_; }

  // Your method here

 protected:
  OmSegmentation* const segmentation_;
  OmSegmentsStore* const store_;
  const std::unique_ptr<OmSegmentSelection> segmentSelection_;
  const std::unique_ptr<OmEnabledSegments> enabledSegments_;

  OmLockedUint32 maxValue_;
  uint32_t mNumSegs;

  QHash<om::common::ID, QString> segmentCustomNames;
  QHash<om::common::ID, QString> segmentNotes;

  inline om::common::SegID getNextValue() { return maxValue_.inc(); }

  void touchFreshness();

  OmSegmentGraph segmentGraph_;

 private:
  friend class OmSegmentSelection;
  friend class OmEnabledSegments;
  friend class SegmentTests;
};
