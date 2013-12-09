#include "segment/lowLevel/omSegmentChildren.hpp"
#include "common/logging.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentIterator.h"
#include "utility/color.hpp"

void OmSegment::setParent(OmSegment* parent, const double threshold) {
  if (parent_) {
    throw om::ArgException("parent already set");
  }

  parent_ = parent;
  threshold_ = threshold;
}

/////////////////////////////////
///////     Color
void OmSegment::RandomizeColor() {
  data_->color = om::utils::color::GetRandomColor(data_->color);
}

void OmSegment::reRandomizeColor() { RandomizeColor(); }

void OmSegment::SetColor(const om::common::Color& color) {
  data_->color = color;
}

void OmSegment::SetColor(const Vector3i& color) {
  data_->color.red = static_cast<uint8_t>(color.x);
  data_->color.green = static_cast<uint8_t>(color.y);
  data_->color.blue = static_cast<uint8_t>(color.z);
}

void OmSegment::SetColor(const Vector3f& color) {
  data_->color.red = static_cast<uint8_t>(color.x * 255);
  data_->color.green = static_cast<uint8_t>(color.y * 255);
  data_->color.blue = static_cast<uint8_t>(color.z * 255);
}

QString OmSegment::GetNote() {
  QString customNote = segments_->getSegmentNote(data_->value);

  if (parent_) {
    customNote += "Parent: " + QString::number(parent_->value()) + "; ";
  }

  // if( !segmentsJoinedIntoMe_.empty() ){
  //   customNote += "Number of Children: "
  //     + QString::number( segmentsJoinedIntoMe_.size() )
  //     + "; ";
  // }

  return customNote;
}

void OmSegment::SetNote(const QString& note) {
  segments_->setSegmentNote(data_->value, note);
}

QString OmSegment::GetName() { return segments_->getSegmentName(data_->value); }

void OmSegment::SetName(const QString& name) {
  segments_->setSegmentName(data_->value, name);
}

bool OmSegment::IsSelected() {
  return segments_->IsSegmentSelected(data_->value);
}

void OmSegment::SetSelected(const bool isSelected, const bool addToRecentList) {
  segments_->setSegmentSelected(data_->value, isSelected, addToRecentList);
}

bool OmSegment::IsEnabled() {
  return segments_->isSegmentEnabled(data_->value);
}

void OmSegment::SetEnabled(const bool isEnabled) {
  segments_->setSegmentEnabled(data_->value, isEnabled);
}

om::common::ID OmSegment::GetSegmentationID() {
  return segments_->GetSegmentationID();
}

om::common::SegID OmSegment::RootID() {
  return segments_->findRootID(data_->value);
}

const segChildCont_t& OmSegment::GetChildren() {
  return segments_->Children()->GetChildren(this);
}

const om::dataBbox OmSegment::BoundingBox() const {
  zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(data_->value);
  return om::dataBbox(data_->bounds, segments_->getSegmentation(), 0);
}
