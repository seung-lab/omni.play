#include "common/debug.h"
#include "segment/segmentsImpl.h"
#include "volume/segmentation.h"
#include "segment/lowLevel/store/segmentStore.h"

// entry into this class via segments hopefully guarantees proper locking...

namespace om {
namespace segment {

segmentsImpl::segmentsImpl(volume::segmentation* segmentation,
                           segmentsStore* segmentPages)
    : segmentsImplLowLevel(segmentation, segmentPages)
{}

segmentsImpl::~segmentsImpl()
{}

segment segmentsImpl::AddSegment()
{
    const common::segId newValue = getNextValue();

    assert(newValue);

    segment newSeg = AddSegment(newValue);
    return newSeg;
}

segment segmentsImpl::AddSegment(const common::segId value)
{
    if(0 == value){
        throw common::argException("bad segId value");
    }

    segment seg = store_->AddSegment(value);

    ++mNumSegs;
    if(maxValue_.get() < value) {
        maxValue_.set(value);
    }

    return seg;
}

void segmentsImpl::Flush(){
    store_->Flush();
}

} // namespace segments
} // namespace om
