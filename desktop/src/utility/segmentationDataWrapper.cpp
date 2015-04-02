#include <QString>

#include "project//details/omProjectVolumes.h"
#include "project/details/omSegmentationManager.h"
#include "project/omProject.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/selection.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "volume/omSegmentation.h"
#include "volume/metadataManager.h"

const om::common::IDSet& SegmentationDataWrapper::ValidIDs() {
  return OmProject::Volumes().Segmentations().GetValidSegmentationIds();
}

const std::vector<OmSegmentation*> SegmentationDataWrapper::GetPtrVec() {
  return OmProject::Volumes().Segmentations().GetPtrVec();
}

void SegmentationDataWrapper::Remove(const om::common::ID id) {
  OmProject::Volumes().Segmentations().RemoveSegmentation(id);
}

SegmentationDataWrapper::SegmentationDataWrapper()
    : id_(0), segmentation_(nullptr) {}

SegmentationDataWrapper::SegmentationDataWrapper(const om::common::ID ID)
    : id_(ID), segmentation_(nullptr) {}

SegmentationDataWrapper::SegmentationDataWrapper(OmSegmentation* vol)
    : id_(vol->id()), segmentation_(vol) {}

SegmentationDataWrapper::SegmentationDataWrapper(
    const SegmentationDataWrapper& sdw)
    : id_(sdw.id_), segmentation_(sdw.segmentation_) {}

void SegmentationDataWrapper::set(const SegmentationDataWrapper& sdw) {
  id_ = sdw.id_;
  segmentation_ = sdw.segmentation_;
}

SegmentationDataWrapper& SegmentationDataWrapper::operator=(
    const SegmentationDataWrapper& sdw) {
  if (this != &sdw) {
    id_ = sdw.id_;
    segmentation_ = sdw.segmentation_;
  }
  return *this;
}

bool SegmentationDataWrapper::operator==(const SegmentationDataWrapper& sdw)
    const {
  return id_ == sdw.id_;
}

bool SegmentationDataWrapper::operator!=(const SegmentationDataWrapper& sdw)
    const {
  return !(*this == sdw);
}

om::common::ID SegmentationDataWrapper::GetSegmentationID() const {
  return id_;
}

om::common::ID SegmentationDataWrapper::id() const { return id_; }

OmSegmentation& SegmentationDataWrapper::Create() {
  OmSegmentation& s = OmProject::Volumes().Segmentations().AddSegmentation();
  id_ = s.id();
  log_debugs << "create segmentation " << id_;
  s.LoadEmptyPath();
  segmentation_ = &s;
  return s;
}

void SegmentationDataWrapper::Remove() {
  Remove(id_);
  id_ = 0;
  segmentation_ = nullptr;
}

om::common::ObjectType SegmentationDataWrapper::getType() const {
  return om::common::SEGMENTATION;
}

bool SegmentationDataWrapper::IsValidWrapper() const {
  return IsSegmentationValid();
}

bool SegmentationDataWrapper::IsSegmentationValid() const {
  if (!id_) {
    return false;
  }

  return OmProject::Volumes().Segmentations().IsSegmentationValid(id_);
}

OmSegmentation* SegmentationDataWrapper::GetSegmentation() const {
  if (!segmentation_) {
    segmentation_ = OmProject::Volumes().Segmentations().GetSegmentation(id_);
  }
  return segmentation_;
}

QString SegmentationDataWrapper::GetName() const {
  auto s = GetSegmentation();
  if (s) {
    return QString::fromStdString(s->GetName());
  } else {
    log_errors << "Invalid Segmentation: " << id_;
    return "";
  }
}

bool SegmentationDataWrapper::isEnabled() const {
  return OmProject::Volumes().Segmentations().IsSegmentationEnabled(id_);
}

OmSegments* SegmentationDataWrapper::Segments() const {
  auto s = GetSegmentation();
  if (s) {
    return &s->Segments();
  } else {
    log_errors << "Invalid Segmentation: " << id_;
    return nullptr;
  }
}

uint32_t SegmentationDataWrapper::getMaxSegmentValue() const {
  auto s = GetSegmentation();
  if (s) {
    return s->Metadata().maxSegments();
  } else {
    log_errors << "Invalid Segmentation: " << id_;
    return 0;
  }
}

OmSegmentLists* SegmentationDataWrapper::SegmentLists() const {
  auto s = Segments();
  if (s) {
    return &s->SegmentLists();
  } else {
    log_errors << "Invalid Segmentation: " << id_;
    return nullptr;
  }
}

OmValidGroupNum* SegmentationDataWrapper::ValidGroupNum() const {
  auto s = GetSegmentation();
  if (s) {
    return &s->ValidGroupNum();
  } else {
    log_errors << "Invalid Segmentation: " << id_;
    return nullptr;
  }
}

const Vector3f SegmentationDataWrapper::GetDataResolution() const {
  auto s = GetSegmentation();
  if (s) {
    return s->Metadata().resolution();
  } else {
    log_errors << "Invalid Segmentation: " << id_;
    return Vector3f::ZERO;
  }
}

const om::common::SegIDSet SegmentationDataWrapper::GetSelectedSegmentIDs()
    const {
  auto s = Segments();
  if (s) {
    return s->Selection().GetSelectedSegmentIDs();
  } else {
    log_errors << "Invalid Segmentation: " << id_;
    return om::common::SegIDSet();
  }
}

bool SegmentationDataWrapper::IsBuilt() const {
  auto s = GetSegmentation();
  if (s) {
    return s->built();
  } else {
    log_errors << "Invalid Segmentation: " << id_;
    return false;
  }
}

uint32_t SegmentationDataWrapper::getNumberOfSegments() const {
  auto s = Segments();
  if (s) {
    return s->GetNumSegments();
  } else {
    log_errors << "Invalid Segmentation: " << id_;
    return 0;
  }
}

uint32_t SegmentationDataWrapper::getNumberOfTopSegments() const {
  auto s = Segments();
  if (s) {
    return s->SegmentLists().GetNumTopLevelSegs();
  } else {
    log_errors << "Invalid Segmentation: " << id_;
    return 0;
  }
}
