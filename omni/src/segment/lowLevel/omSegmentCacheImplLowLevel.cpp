#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/omSegmentLists.hpp"
#include "system/cache/omCacheManager.h"
#include "volume/omSegmentation.h"

OmSegmentCacheImplLowLevel::OmSegmentCacheImplLowLevel(OmSegmentation* segmentation)
    : segmentation_(segmentation)
    , segmentPages_(new OmPagingPtrStore(segmentation))
    , segmentSelection_(new OmSegmentSelection(this))
    , enabledSegments_(new OmEnabledSegments(this))
    , mMaxValue(0)
    , mNumSegs(0)
{}

OmSegmentCacheImplLowLevel::~OmSegmentCacheImplLowLevel()
{}

OmSegID OmSegmentCacheImplLowLevel::GetNumTopSegments(){
    return mSegmentGraph.GetNumTopLevelSegs();
}

void OmSegmentCacheImplLowLevel::addToRecentMap(const OmSegID segID){
    segmentation_->SegmentLists()->TouchRecentList(segID);
}

QString OmSegmentCacheImplLowLevel::getSegmentName(OmSegID segID)
{
    if(segmentCustomNames.empty()){
        return "";
    }

    if(segmentCustomNames.contains(segID)){
        return segmentCustomNames.value(segID);
    }

    return ""; //QString("segment%1").arg(segID);
}

void OmSegmentCacheImplLowLevel::setSegmentName(OmSegID segID, QString name){
    segmentCustomNames[ segID ] = name;
}

QString OmSegmentCacheImplLowLevel::getSegmentNote(OmSegID segID)
{
    if(segmentNotes.empty()){
        return "";
    }

    if(segmentNotes.contains(segID)){
        return segmentNotes.value(segID);
    }

    return "";
}

void OmSegmentCacheImplLowLevel::setSegmentNote(OmSegID segID, QString note){
    segmentNotes[ segID ] = note;
}

quint32 OmSegmentCacheImplLowLevel::getPageSize(){
    return segmentPages_->getPageSize();
}

void OmSegmentCacheImplLowLevel::touchFreshness(){
    OmCacheManager::TouchFresheness();
}

void OmSegmentCacheImplLowLevel::growGraphIfNeeded(OmSegment* newSeg){
    mSegmentGraph.GrowGraphIfNeeded(newSeg);
}
