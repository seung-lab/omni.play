#include "utility/segmentationDataWrapper.hpp"
#include "segment/lowLevel/enabledSegments.hpp"
#include "segment/lowLevel/segmentsImplLowLevel.h"
#include "segment/lowLevel/segmentSelection.hpp"
#include "segment/lists/segmentLists.h"
#include "system/cache/omCacheManager.h"
#include "volume/segmentation.h"

segmentsImplLowLevel::segmentsImplLowLevel(segmentation* segmentation,
                                               segmentsStore* segmentPages)
    : segmentation_(segmentation)
    , store_(segmentPages)
    , segmentSelection_(new segmentSelection(this))
    , enabledSegments_(new enabledSegments(this))
    , mNumSegs(0)
{
    maxValue_.set(0);
}

segmentsImplLowLevel::~segmentsImplLowLevel()
{}

std::string segmentsImplLowLevel::getSegmentName(common::segId segID)
{
    if(segmentCustomNames.empty()){
        return "";
    }

    if(segmentCustomNames.contains(segID)){
        return segmentCustomNames.value(segID);
    }

    return ""; //std::string("segment%1").arg(segID);
}

void segmentsImplLowLevel::setSegmentName(common::segId segID, std::string name){
    segmentCustomNames[ segID ] = name;
}

std::string segmentsImplLowLevel::getSegmentNote(common::segId segID)
{
    if(segmentNotes.empty()){
        return "";
    }

    if(segmentNotes.contains(segID)){
        return segmentNotes.value(segID);
    }

    return "";
}

void segmentsImplLowLevel::setSegmentNote(common::segId segID, std::string note){
    segmentNotes[ segID ] = note;
}

void segmentsImplLowLevel::touchFreshness(){
    OmCacheManager::TouchFreshness();
}

void segmentsImplLowLevel::growGraphIfNeeded(segment* newSeg){
    segmentGraph_.GrowGraphIfNeeded(newSeg);
}

SegmentationDataWrapper segmentsImplLowLevel::GetSDW() const {
    return segmentation_->GetSDW();
}
