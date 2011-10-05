#include "utility/segmentationDataWrapper.hpp"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/segmentsImplLowLevel.h"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/lists/omSegmentLists.h"
#include "system/cache/omCacheManager.h"
#include "volume/segmentation.h"

segmentsImplLowLevel::segmentsImplLowLevel(segmentation* segmentation,
                                               segmentsStore* segmentPages)
    : segmentation_(segmentation)
    , store_(segmentPages)
    , segmentSelection_(new OmSegmentSelection(this))
    , enabledSegments_(new OmEnabledSegments(this))
    , mNumSegs(0)
{
    maxValue_.set(0);
}

segmentsImplLowLevel::~segmentsImplLowLevel()
{}

QString segmentsImplLowLevel::getSegmentName(segId segID)
{
    if(segmentCustomNames.empty()){
        return "";
    }

    if(segmentCustomNames.contains(segID)){
        return segmentCustomNames.value(segID);
    }

    return ""; //QString("segment%1").arg(segID);
}

void segmentsImplLowLevel::setSegmentName(segId segID, QString name){
    segmentCustomNames[ segID ] = name;
}

QString segmentsImplLowLevel::getSegmentNote(segId segID)
{
    if(segmentNotes.empty()){
        return "";
    }

    if(segmentNotes.contains(segID)){
        return segmentNotes.value(segID);
    }

    return "";
}

void segmentsImplLowLevel::setSegmentNote(segId segID, QString note){
    segmentNotes[ segID ] = note;
}

void segmentsImplLowLevel::touchFreshness(){
    OmCacheManager::TouchFreshness();
}

void segmentsImplLowLevel::growGraphIfNeeded(OmSegment* newSeg){
    segmentGraph_.GrowGraphIfNeeded(newSeg);
}

SegmentationDataWrapper segmentsImplLowLevel::GetSDW() const {
    return segmentation_->GetSDW();
}
