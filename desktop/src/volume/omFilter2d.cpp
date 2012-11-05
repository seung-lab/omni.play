#include "utility/channelDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "volume/omFilter2d.h"

OmFilter2d::OmFilter2d()
    : alpha_(0.0)
    , filterType_(om::OVERLAY_NONE)
    , chanID_(0)
    , segID_(0)
{}

OmFilter2d::OmFilter2d(const om::common::ID filterID)
    : OmManageableObject(filterID)
    , alpha_(0.0)
    , filterType_(om::OVERLAY_NONE)
    , chanID_(0)
    , segID_(1)
{}

void OmFilter2d::Load()
{
    if(chanID_){
        SetChannel(chanID_);

    } else if(segID_){
        SetSegmentation(segID_);
    }
}

void OmFilter2d::reset()
{
    chanID_ = 0;
    channVolPtr_.reset();

    segID_ = 0;
    segVolPtr_.reset();

    filterType_ = om::OVERLAY_NONE;
}

void OmFilter2d::SetSegmentation(const om::common::ID id)
{
    reset();

    SegmentationDataWrapper sdw(id);
    if(!sdw.IsSegmentationValid()){
        return;
    }

    segVolPtr_ = boost::optional<OmSegmentation*>(sdw.GetSegmentationPtr());
    filterType_ = om::OVERLAY_SEGMENTATION;
    segID_ = id;
}

void OmFilter2d::SetChannel(const om::common::ID id)
{
    reset();

    ChannelDataWrapper cdw(id);
    if(!cdw.IsChannelValid()){
        return;
    }

    channVolPtr_ = boost::optional<OmChannel*>(cdw.GetChannelPtr());
    filterType_ = om::OVERLAY_CHANNEL;
    chanID_ = id;
}
