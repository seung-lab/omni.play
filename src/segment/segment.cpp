#include "common/debug.h"
#include "segment/segment.h"
#include "segment/segments.h"
#include "volume/segmentation.h"

namespace om {
namespace segment {

std::string segment::GetNote()
{
    std::string customNote = segments_->getSegmentNote(data_->value);

    return customNote;
}

void segment::SetNote(const std::string& note){
    segments_->setSegmentNote( data_->value, note );
}

std::string segment::GetName(){
    return segments_->getSegmentName( data_->value );
}

void segment::SetName(const std::string& name){
    segments_->setSegmentName( data_->value, name );
}

common::id segment::GetSegmentationID(){
    return segments_->GetSegmentationID();
}

const coords::dataBbox segment::BoundingBox() const
{
    zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(data_->value);
    return coords::dataBbox(data_->bounds, *segments_->getSegmentation(), 0);
}

} // namespace segments
} // namespace om
