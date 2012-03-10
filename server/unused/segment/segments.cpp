#include "datalayer/dataPaths.h"
#include "segment/segment.h"
#include "segment/segments.h"
#include "segment/segmentsImpl.h"
#include "volume/segmentation.h"
#include "segment/lowLevel/store/segmentStore.h"

namespace om {
namespace segment {

segments::segments(volume::segmentation* segmentation)
    : segmentation_(segmentation)
    , store_(new segmentsStore(segmentation))
    , impl_(new segmentsImpl(segmentation, store_.get()))
{}

segments::~segments()
{}

common::segId segments::GetSegmentationID(){
    return segmentation_->GetID();
}

uint32_t segments::getPageSize()
{
    // locked internally
    return store_->PageSize();
}

segment segments::AddSegment()
{
    zi::guard g(mutex_);
    return impl_->AddSegment();
}

segment segments::AddSegment(common::segId value)
{
    zi::guard g(mutex_);
    return impl_->AddSegment(value);
}

bool segments::IsSegmentValid(common::segId seg)
{
    // locked internally
    return store_->IsSegmentValid(seg);
}

segment segments::GetSegmentUnsafe(const common::segId value)
{
    // locked internally
    return store_->GetSegmentUnsafe(value);
}

common::segId segments::GetNumSegments()
{
    zi::guard g(mutex_);
    return impl_->GetNumSegments();
}

void segments::setSegmentName(common::segId segID, std::string name)
{
    zi::guard g(mutex_);
    impl_->setSegmentName(segID, name);
}

std::string segments::getSegmentName(common::segId segID)
{
    zi::guard g(mutex_);
    return impl_->getSegmentName(segID);
}

void segments::setSegmentNote(common::segId segID, std::string note)
{
    zi::guard g(mutex_);
    impl_->setSegmentNote(segID, note);
}

std::string segments::getSegmentNote(common::segId segID)
{
    zi::guard g(mutex_);
    return impl_->getSegmentNote(segID);
}

uint32_t segments::getMaxValue()
{
    // self-locking integer
    return impl_->getMaxValue();
}

void segments::Flush()
{
    zi::guard g(mutex_);
    return impl_->Flush();
}

} // namespace segment
} // namespace om
