#pragma once
#include "precomp.h"

#include "common/common.h"
#include "common/colors.h"

class QString;
class SegmentationDataWrapper;
class OmSegmentation;
class OmSegment;
class OmSegmentLists;
class OmSegments;
namespace om {
namespace coords {
class Global;
}
namespace segment {
class Selection;
}
}

class SegmentDataWrapper {
 private:
  om::common::SegID segmentID_;
  om::common::ID segmentationID_;

  // not allowed--allows wrappers to be implicitly converted!!!
  explicit SegmentDataWrapper(const SegmentationDataWrapper& sdw);

 public:
  SegmentDataWrapper();
  explicit SegmentDataWrapper(const om::common::ID segmentationID,
                              const om::common::SegID segmentID);
  explicit SegmentDataWrapper(const OmSegmentation* segmentation,
                              const om::common::SegID segmentID);
  explicit SegmentDataWrapper(const SegmentationDataWrapper& sdw,
                              const om::common::SegID segID);

  void set(const SegmentDataWrapper& sdw);
  void SetSegmentID(const om::common::SegID segID);

  SegmentDataWrapper& operator=(const SegmentDataWrapper& sdw);
  bool operator<(const SegmentDataWrapper& rhs) const;
  om::common::SegID GetSegmentID() const;
  bool operator==(const SegmentDataWrapper& sdw) const;
  bool operator!=(const SegmentDataWrapper& sdw) const;

  bool IsSegmentationValid() const;
  bool IsSegmentValid() const;
  bool IsValidWrapper() const;

  SegmentationDataWrapper MakeSegmentationDataWrapper() const;
  QString GetSegmentationName() const;
  om::common::ID GetSegmentationID() const;
  OmSegmentation* GetSegmentation() const;

  OmSegment* GetSegment() const;
  OmSegments* Segments() const;
  om::segment::Selection* Selection() const;
  OmSegmentLists* SegmentLists() const;

  om::common::SegID FindRootID() const;
  OmSegment* FindRoot() const;
  SegmentDataWrapper FindRootSDW() const;

  om::common::SegID GetID() const;
  om::common::SegID GetVoxelValue(const om::coords::Global& clickPoint) const;

  QString GetName() const;
  void SetName(const QString& str) const;

  uint64_t GetSize() const;
  int64_t GetSizeWithChildren() const;

  bool isSelected() const;
  void setSelected(const bool isSelected, const bool addToRecentList) const;

  QString GetNote() const;
  void SetNote(const QString& str) const;

  om::common::Color GetColorInt() const;
  Vector3f GetColorFloat() const;
  void SetColor(const om::common::Color& color) const;
  void SetColor(const Vector3f& color) const;
  void RandomizeColor() const;

  friend std::ostream& operator<<(std::ostream& out,
                                  const SegmentDataWrapper& s);
};
