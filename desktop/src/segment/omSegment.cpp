#include "segment/lowLevel/children.hpp"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentIterator.h"
#include "utility/color.hpp"
#include "volume/omSegmentation.h"

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
  data_.color = om::utils::color::GetRandomColor(data_.color);

  log_debugs << "final color values: " << (int)data_.color.red << ","
             << (int)data_.color.green << "," << (int)data_.color.blue << "\n";
}

void OmSegment::reRandomizeColor() { RandomizeColor(); }

void OmSegment::SetColor(const om::common::Color& color) {
  data_.color = color;
}

void OmSegment::SetColor(const Vector3i& color) {
  data_.color.red = static_cast<uint8_t>(color.x);
  data_.color.green = static_cast<uint8_t>(color.y);
  data_.color.blue = static_cast<uint8_t>(color.z);
}

void OmSegment::SetColor(const Vector3f& color) {
  data_.color.red = static_cast<uint8_t>(color.x * 255);
  data_.color.green = static_cast<uint8_t>(color.y * 255);
  data_.color.blue = static_cast<uint8_t>(color.z * 255);
}

const om::coords::DataBbox OmSegment::BoundingBox() const {
  zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(data_.value);
  return om::coords::DataBbox(data_.bounds, system_, 0);
}
