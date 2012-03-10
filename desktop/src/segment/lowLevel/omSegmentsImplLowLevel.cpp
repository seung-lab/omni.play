#include "utility/segmentationDataWrapper.hpp"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/lists/omSegmentLists.h"
#include "system/cache/omCacheManager.h"
#include "volume/omSegmentation.h"

OmSegmentsImplLowLevel::OmSegmentsImplLowLevel(OmSegmentation* segmentation,
                                               OmSegmentsStore* segmentPages)
    : segmentation_(segmentation)
    , store_(segmentPages)
    , segmentSelection_(new OmSegmentSelection(this))
    , enabledSegments_(new OmEnabledSegments(this))
    , mNumSegs(0)
{
    maxValue_.set(0);
}

OmSegmentsImplLowLevel::~OmSegmentsImplLowLevel()
{}

QString OmSegmentsImplLowLevel::getSegmentName(OmSegID segID)
{
    if(segmentCustomNames.empty()){
        return "";
    }

    if(segmentCustomNames.contains(segID)){
        return segmentCustomNames.value(segID);
    }

    return ""; //QString("segment%1").arg(segID);
}

void OmSegmentsImplLowLevel::setSegmentName(OmSegID segID, QString name){
    segmentCustomNames[ segID ] = name;
}

QString OmSegmentsImplLowLevel::getSegmentNote(OmSegID segID)
{
    if(segmentNotes.empty()){
        return "";
    }

    if(segmentNotes.contains(segID)){
        return segmentNotes.value(segID);
    }

    return "";
}

void OmSegmentsImplLowLevel::setSegmentNote(OmSegID segID, QString note){
    segmentNotes[ segID ] = note;
}

void OmSegmentsImplLowLevel::touchFreshness(){
    OmCacheManager::TouchFreshness();
}

void OmSegmentsImplLowLevel::growGraphIfNeeded(OmSegment* newSeg){
    segmentGraph_.GrowGraphIfNeeded(newSeg);
}

SegmentationDataWrapper OmSegmentsImplLowLevel::GetSDW() const {
    return segmentation_->GetSDW();
}
