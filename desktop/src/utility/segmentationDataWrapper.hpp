#pragma once
#include "precomp.h"

#include "common/common.h"

class OmSegment;
class OmSegmentLists;
class OmSegmentation;
class OmSegments;
class OmValidGroupNum;
class QString;
class SegmentDataWrapper;

class SegmentationDataWrapper {
 public:
  static const om::common::IDSet& ValidIDs();
  static const std::vector<OmSegmentation*> GetPtrVec();
  static void Remove(const om::common::ID id);

 private:
  om::common::ID id_;
  mutable OmSegmentation* segmentation_;

 public:
  SegmentationDataWrapper();
  explicit SegmentationDataWrapper(const om::common::ID id);
  explicit SegmentationDataWrapper(OmSegmentation*);
  SegmentationDataWrapper(const SegmentationDataWrapper& sdw);

  void set(const SegmentationDataWrapper& sdw);
  SegmentationDataWrapper& operator=(const SegmentationDataWrapper& sdw);
  bool operator==(const SegmentationDataWrapper& sdw) const;
  bool operator!=(const SegmentationDataWrapper& sdw) const;

  om::common::ID GetSegmentationID() const;
  om::common::ID id() const;
  OmSegmentation& Create();
  void Remove();
  om::common::ObjectType getType() const;
  bool IsValidWrapper() const;
  bool IsSegmentationValid() const;
  OmSegmentation* GetSegmentation() const;
  QString GetName() const;
  bool isEnabled() const;
  uint32_t getNumberOfSegments() const;
  uint32_t getNumberOfTopSegments() const;
  OmSegments* Segments() const;
  uint32_t getMaxSegmentValue() const;
  OmSegmentLists* SegmentLists() const;
  OmValidGroupNum* ValidGroupNum() const;
  const Vector3f GetDataResolution() const;
  const om::common::SegIDSet GetSelectedSegmentIDs() const;
  bool IsBuilt() const;
};
