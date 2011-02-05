#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "system/cache/omCacheManager.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentLists.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/lowLevel/omSegmentSelection.hpp"

// entry into this class via OmSegmentCache hopefully guarentees proper locking...

OmSegmentCacheImplLowLevel::OmSegmentCacheImplLowLevel(OmSegmentation* segmentation)
    : segmentation_(segmentation)
    , mSegments(new OmPagingPtrStore(segmentation))
    , mMaxValue(0)
    , mNumSegs(0)
    , segmentSelection_(new OmSegmentSelection(this))
    , mAllEnabled(false)
{}

OmSegmentCacheImplLowLevel::~OmSegmentCacheImplLowLevel()
{}

OmSegment* OmSegmentCacheImplLowLevel::findRoot(OmSegment* segment)
{
    if(!segment){
        return 0;
    }

    if(!segment->getParent()) {
        return segment;
    }

    return GetSegment(mSegmentGraph.graph_getRootID(segment->value()));
}

OmSegment* OmSegmentCacheImplLowLevel::findRoot(const OmSegID segID)
{
    if(!segID){
        return 0;
    }

    return GetSegment(mSegmentGraph.graph_getRootID(segID));
}

OmSegID OmSegmentCacheImplLowLevel::findRootID(const OmSegID segID)
{
    if(!segID){
        return 0;
    }

    return mSegmentGraph.graph_getRootID(segID);
}

OmSegID OmSegmentCacheImplLowLevel::findRootID(OmSegment* segment)
{
    if(!segment){
        return 0;
    }

    if(!segment->getParent()) {
        return segment->value();
    }

    return mSegmentGraph.graph_getRootID(segment->value());
}

OmSegment* OmSegmentCacheImplLowLevel::GetSegment(const OmSegID value)
{
    OmSegment* seg = mSegments->GetSegment(value);

    if(seg && seg->value() > mMaxValue){
        throw OmIoException("bad segment value: "
                            + om::string::num(seg->value())
                            + "; maxValue is: "
                            + om::string::num(mMaxValue));
    }

    return seg;
}

OmSegID OmSegmentCacheImplLowLevel::GetNumSegments()
{
    return mNumSegs;
}

OmSegID OmSegmentCacheImplLowLevel::GetNumTopSegments()
{
    return mSegmentGraph.getNumTopLevelSegs();
}

bool OmSegmentCacheImplLowLevel::isSegmentEnabled(OmSegID segID)
{
    const OmSegID rootID = findRoot(GetSegment(segID))->value();

    if(mAllEnabled ||
       0 != mEnabledSet.count(rootID)){
        return true;
    }

    return false;
}

void OmSegmentCacheImplLowLevel::setSegmentEnabled(OmSegID segID, bool isEnabled)
{
    const OmSegID rootID = findRoot(GetSegment(segID))->value();
    touchFreshness();

    if (isEnabled) {
        mEnabledSet.insert(rootID);
    } else {
        mEnabledSet.erase(rootID);
    }
}

void OmSegmentCacheImplLowLevel::SetAllEnabled(bool enabled)
{
    mAllEnabled = enabled;
    mEnabledSet.clear();
}

OmSegIDsSet & OmSegmentCacheImplLowLevel::GetEnabledSegmentIdsRef()
{
    return mEnabledSet;
}


uint32_t OmSegmentCacheImplLowLevel::numberOfEnabledSegments()
{
    return mEnabledSet.size();
}

void OmSegmentCacheImplLowLevel::addToRecentMap(const OmSegID segID)
{
    segmentation_->SegmentLists()->TouchRecentList(segID);
}

QString OmSegmentCacheImplLowLevel::getSegmentName(OmSegID segID)
{
    if(segmentCustomNames.contains(segID)){
        return segmentCustomNames.value(segID);
    }

    return ""; //QString("segment%1").arg(segID);
}

void OmSegmentCacheImplLowLevel::setSegmentName(OmSegID segID, QString name)
{
    segmentCustomNames[ segID ] = name;
}

QString OmSegmentCacheImplLowLevel::getSegmentNote(OmSegID segID)
{
    if(segmentNotes.contains(segID)){
        return segmentNotes.value(segID);
    }

    return "";
}

void OmSegmentCacheImplLowLevel::setSegmentNote(OmSegID segID, QString note)
{
    segmentNotes[ segID ] = note;
}

OmSegID OmSegmentCacheImplLowLevel::GetSegmentationID()
{
    return segmentation_->GetID();
}

quint32 OmSegmentCacheImplLowLevel::getPageSize()
{
    return mSegments->getPageSize();
}

uint32_t OmSegmentCacheImplLowLevel::getMaxValue()
{
    return mMaxValue;
}

OmSegID OmSegmentCacheImplLowLevel::getNextValue()
{
    ++mMaxValue;
    return mMaxValue;
}

void OmSegmentCacheImplLowLevel::touchFreshness(){
    OmCacheManager::TouchFresheness();
}

void OmSegmentCacheImplLowLevel::growGraphIfNeeded(OmSegment* newSeg)
{
    mSegmentGraph.growGraphIfNeeded(newSeg);
}

OmSegmentCache* OmSegmentCacheImplLowLevel::SegmentCache()
{
    return segmentation_->SegmentCache();
}

bool OmSegmentCacheImplLowLevel::AreSegmentsEnabled()
{
    if(0 == numberOfEnabledSegments()){
        return false;
    }

    return true;
}

