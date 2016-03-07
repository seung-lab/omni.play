#include <QString>

#include "segment/omSegment.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omSegmentationManager.h"
#include "project/omProject.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegments.h"
#include "segment/selection.hpp"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "volume/omSegmentation.h"

SegmentDataWrapper::SegmentDataWrapper() : segmentID_(0), segmentationID_(0) {}

SegmentDataWrapper::SegmentDataWrapper(const om::common::ID segmentationID,
                                       const om::common::SegID segmentID)
    : segmentID_(segmentID), segmentationID_(segmentationID) {}

SegmentDataWrapper::SegmentDataWrapper(const OmSegmentation* segmentation,
                                       const om::common::SegID segmentID)
    : segmentID_(segmentID), segmentationID_(segmentation->id()) {}

SegmentDataWrapper::SegmentDataWrapper(const SegmentationDataWrapper& sdw,
                                       const om::common::SegID segID)
    : segmentID_(segID), segmentationID_(sdw.GetSegmentationID()) {}

void SegmentDataWrapper::set(const SegmentDataWrapper& sdw) {
  segmentID_ = sdw.segmentID_;
  segmentationID_ = sdw.segmentationID_;
}

void SegmentDataWrapper::SetSegmentID(const om::common::SegID segID) {
  segmentID_ = segID;
}

SegmentDataWrapper& SegmentDataWrapper::operator=(
    const SegmentDataWrapper& sdw) {
  if (this != &sdw) {
    segmentID_ = sdw.segmentID_;
    segmentationID_ = sdw.segmentationID_;
  }

  return *this;
}

bool SegmentDataWrapper::operator<(const SegmentDataWrapper& rhs) const {
  if (segmentationID_ != rhs.segmentationID_) {
    return segmentationID_ < rhs.segmentationID_;
  }

  return segmentID_ < rhs.segmentID_;
}

om::common::SegID SegmentDataWrapper::GetSegmentID() const {
  return segmentID_;
}

bool SegmentDataWrapper::operator==(const SegmentDataWrapper& sdw) const {
  return segmentID_ == sdw.segmentID_ && segmentationID_ == sdw.segmentationID_;
}

bool SegmentDataWrapper::operator!=(const SegmentDataWrapper& sdw) const {
  return !(*this == sdw);
}

SegmentationDataWrapper SegmentDataWrapper::MakeSegmentationDataWrapper()
    const {
  return SegmentationDataWrapper(segmentationID_);
}

bool SegmentDataWrapper::IsSegmentationValid() const {
  if (!segmentationID_) {
    return false;
  }

  return OmProject::Volumes().Segmentations().IsSegmentationValid(
      segmentationID_);
}

bool SegmentDataWrapper::IsSegmentValid() const {
  if (!segmentID_ && !segmentationID_) {
    return false;
  }

  return IsSegmentationValid() && Segments()->IsSegmentValid(segmentID_);
}

bool SegmentDataWrapper::IsValidWrapper() const { return IsSegmentValid(); }

QString SegmentDataWrapper::GetSegmentationName() const {
  auto seg = GetSegmentation();
  if (seg) {
    return QString::fromStdString(GetSegmentation()->GetName());
  } else {
    return "";
  }
}

bool SegmentDataWrapper::isSelected() const {
  auto s = Selection();
  if (s) {
    return s->IsSegmentSelected(segmentID_);
  } else {
    return false;
  }
}

void SegmentDataWrapper::setSelected(const bool isSelected,
                                     const bool addToRecentList) const {
  auto s = Selection();
  if (s) {
    s->setSegmentSelected(segmentID_, isSelected, addToRecentList);
  }
}

QString SegmentDataWrapper::GetNote() const {
  auto s = Segments();
  if (s) {
    return QString::fromStdString(s->GetNote(segmentID_));
  } else {
    return "";
  }
}

void SegmentDataWrapper::SetNote(const QString& str) const {
  auto s = Segments();
  if (s) {
    s->SetNote(segmentID_, str.toStdString());
  }
}

om::common::Color SegmentDataWrapper::GetColorInt() const {
  auto s = GetSegment();
  if (s) {
    return s->GetColorInt();
  } else {
    return om::common::Color();
  }
}

Vector3f SegmentDataWrapper::GetColorFloat() const {
  auto s = GetSegment();
  if (s) {
    return s->GetColorFloat();
  } else {
    return Vector3f::ZERO;
  }
}

void SegmentDataWrapper::SetColor(const om::common::Color& color) const {
  auto s = GetSegment();
  if (s) {
    s->SetColor(color);
  }
}

void SegmentDataWrapper::SetColor(const Vector3f& color) const {
  auto s = GetSegment();
  if (s) {
    s->SetColor(color);
  }
}

QString SegmentDataWrapper::GetName() const {
  auto s = Segments();
  if (s) {
    return QString::fromStdString(s->GetName(segmentID_));
  } else {
    return "";
  }
}

void SegmentDataWrapper::SetName(const QString& str) const {
  auto s = Segments();
  if (s) {
    s->SetName(segmentID_, str.toStdString());
  }
}

QString SegmentDataWrapper::GetTags() const {
  auto s = Segments();
  if (s) {
    return QString::fromStdString(s->GetTags(segmentID_));
  } else {
    return "";
  }
}

void SegmentDataWrapper::SetTags(const QString& str) const {
  auto s = Segments();
  if (s) {
    s->SetTags(segmentID_, str.toStdString());
  }
}

OmSegmentation* SegmentDataWrapper::GetSegmentation() const {
  return OmProject::Volumes().Segmentations().GetSegmentation(segmentationID_);
}

OmSegment* SegmentDataWrapper::GetSegment() const {
  auto s = Segments();
  if (s) {
    return s->GetSegment(segmentID_);
  } else {
    log_errors << "Invalid Segment: " << segmentationID_ << ":" << segmentID_;
    return nullptr;
  }
}

OmSegments* SegmentDataWrapper::Segments() const {
  auto seg = GetSegmentation();
  if (seg) {
    return &seg->Segments();
  } else {
    log_errors << "Invalid Segments: " << segmentationID_;
    return nullptr;
  }
}

om::segment::Selection* SegmentDataWrapper::Selection() const {
  auto seg = GetSegmentation();
  if (seg) {
    return &seg->Segments().Selection();
  } else {
    log_errors << "Invalid Selection: " << segmentationID_;
    return nullptr;
  }
}

uint64_t SegmentDataWrapper::GetSize() const { return GetSegment()->size(); }

om::common::ID SegmentDataWrapper::GetSegmentationID() const {
  return segmentationID_;
}

om::common::SegID SegmentDataWrapper::GetID() const { return segmentID_; }

om::common::SegID SegmentDataWrapper::GetVoxelValue(
    const om::coords::Global& dataClickPoint) const {
  auto seg = GetSegmentation();
  if (seg) {
    return seg->GetVoxelValue(dataClickPoint);
  } else {
    return 0;
  }
}

om::common::SegID SegmentDataWrapper::FindRootID() const {
  auto s = Segments();
  if (s) {
    return s->FindRootID(segmentID_);
  } else {
    return 0;
  }
}

OmSegment* SegmentDataWrapper::FindRoot() const {
  auto s = Segments();
  if (s) {
    return s->FindRoot(segmentID_);
  } else {
    return nullptr;
  }
}

void SegmentDataWrapper::RandomizeColor() const {
  auto s = GetSegment();
  if (s) {
    s->reRandomizeColor();
  }
}

int64_t SegmentDataWrapper::GetSizeWithChildren() const {
  auto s = SegmentLists();
  if (s) {
    return s->GetSizeWithChildren(segmentID_);
  } else {
    return 0;
  }
}

SegmentDataWrapper SegmentDataWrapper::FindRootSDW() const {
  return SegmentDataWrapper(segmentationID_, FindRootID());
}

std::ostream& operator<<(std::ostream& out, const SegmentDataWrapper& s) {
  out << "(segmentation " << s.segmentationID_ << ", segment " << s.segmentID_
      << ")";
  return out;
}

OmSegmentLists* SegmentDataWrapper::SegmentLists() const {
  auto seg = GetSegmentation();
  if (seg) {
    return &seg->Segments().SegmentLists();
  } else {
    log_errors << "Invalid SegmentLists: " << segmentationID_;
    return nullptr;
  }
}
