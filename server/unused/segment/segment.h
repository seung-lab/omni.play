#pragma once

/*
 * Segment Object
 *
 * Stored in object pool; values are initialized to 0, except for edgeNumber_
 *
 * Brett Warne - bwarne@mit.edu - 3/9/09
 * Michael Purcaro - purcaro@gmail.com
 */

#include "segment/segmentTypes.h"
#include "segment/segmentChildrenTypes.h"
#include "zi/mutex.h"

namespace om {
namespace segment {

class segments;

class segment {
public:
    segment()
        : data_(NULL)
        , segments_(NULL)
    {}

    segment(data* data, segments* segments)
        : data_(data)
        , segments_(segments)
    {}

    inline common::segId value() const {
        return data_->value;
    }

    std::string GetNote();
    void SetNote(const std::string&);
    std::string GetName();
    void SetName(const std::string&);

    uint64_t size() const {
        return data_->size;
    }

    inline void addToSize(const uint64_t inc)
    {
        zi::spinlock::pool<segment_size_mutex_pool_tag>::guard g(data_->value);
        data_->size += inc;
    }

    common::id GetSegmentationID();

    const coords::dataBbox BoundingBox() const;

    inline void ClearBoundingBox()
    {
        zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(data_->value);
        data_->bounds.set(Vector3i::ZERO, Vector3i::ZERO);
    }

    inline void AddToBoundingBox(const coords::dataBbox& box)
    {
        zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(data_->value);
        data_->bounds.merge(box);
    }

    inline segments* Segments(){
        return segments_;
    }

private:
    data* data_;
    segments* segments_;

    struct segment_bounds_mutex_pool_tag;
    struct segment_size_mutex_pool_tag;
};

} // namespace segment
} // namespace om
