#pragma once
#include "precomp.h"

#include "common/enums.hpp"
#include "coordinates/coordinates.h"
#include "segment/omSegmentTypes.h"
#include "segment/types.hpp"

class OmSegments;

class OmSegment {
 public:
  OmSegment(om::segment::Data& data, om::common::SegListType& listType,
            const om::coords::VolumeSystem& system)
      : data_(&data),
        listType_(&listType),
        system_(&system),
        parent_(nullptr),
        threshold_(0),
        edgeNumber_(-1),
        freshnessForMeshes_(0) {}

  OmSegment(const OmSegment& other)
      : data_(other.data_),
        listType_(other.listType_),
        system_(other.system_),
        parent_(other.parent_),
        threshold_(other.threshold_),
        edgeNumber_(other.edgeNumber_),
        freshnessForMeshes_(other.freshnessForMeshes_) {}

  friend void swap(OmSegment& a, OmSegment& b) {
    using std::swap;
    swap(a.data_, b.data_);
    swap(a.listType_, b.listType_);
    swap(a.system_, b.system_);
    swap(a.parent_, b.parent_);
    swap(a.threshold_, b.threshold_);
    swap(a.edgeNumber_, b.edgeNumber_);
    swap(a.freshnessForMeshes_, b.freshnessForMeshes_);
  }

  OmSegment(OmSegment&& other) { swap(*this, other); }

  OmSegment& operator=(OmSegment other) {
    swap(*this, other);
    return *this;
  }

  om::common::SegID value() const { return data_->value; }

  // color
  void RandomizeColor();

  void reRandomizeColor();

  om::common::Color GetColorInt() const { return data_->color; }

  Vector3f GetColorFloat() const {
    return Vector3f(data_->color.red / 255., data_->color.green / 255.,
                    data_->color.blue / 255.);
  }

  void SetColor(const om::common::Color&);
  void SetColor(const Vector3i&);
  void SetColor(const Vector3f&);

  uint64_t size() const { return data_->size; }

  void addToSize(const uint64_t inc) {
    zi::spinlock::pool<segment_size_mutex_pool_tag>::guard g(data_->value);
    data_->size += inc;
  }

  inline bool IsValidListType() const {
    return om::common::SegListType::VALID ==
           static_cast<om::common::SegListType>(*listType_);
  }

  om::common::SegListType GetListType() const {
    return static_cast<om::common::SegListType>(*listType_);
  }

  void SetListType(const om::common::SegListType type) { *listType_ = type; }

  OmSegment* getParent() const { return parent_; }

  void setParent(OmSegment* seg) { parent_ = seg; }

  void setParent(OmSegment* segment, const double);

  double getThreshold() const { return threshold_; }

  void setThreshold(const double thres) { threshold_ = thres; }

  const om::coords::DataBbox BoundingBox() const;

  void ClearBoundingBox() {
    zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(data_->value);
    data_->bounds.set(Vector3i::ZERO, Vector3i::ZERO);
  }

  void AddToBoundingBox(const om::coords::DataBbox& box) {
    zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(data_->value);
    data_->bounds.merge(box);
  }

  uint64_t getFreshnessForMeshes() const { return freshnessForMeshes_; }

  void touchFreshnessForMeshes() { ++freshnessForMeshes_; }

  int getEdgeNumber() const { return edgeNumber_; }

  void setEdgeNumber(const int num) { edgeNumber_ = num; }

  const om::segment::UserEdge& getCustomMergeEdge() const {
    return customMergeEdge_;
  }

  void setCustomMergeEdge(const om::segment::UserEdge& e) {
    customMergeEdge_ = e;
  }

 private:
  om::segment::Data* data_;
  om::common::SegListType* listType_;
  const om::coords::VolumeSystem* system_;

  OmSegment* parent_;
  double threshold_;

  int edgeNumber_;  // index of edge in main edge list

  om::segment::UserEdge customMergeEdge_;
  uint64_t freshnessForMeshes_;

  struct segment_bounds_mutex_pool_tag;
  struct segment_size_mutex_pool_tag;

  // TODO: do something about this.  Look at OmSegmentPage.  Prety crazy
  // initialization...
  friend class OmSegmentPage;
  friend class OmPagingPtrStore;
};
