#include "datalayer/omDataPath.h"
#include "datalayer/omDataPath.h"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "volume/omSegmentation.h"

OmSegmentCache::OmSegmentCache(OmSegmentation* segmentation)
    : segmentation_(segmentation)
    , mImpl(new OmSegmentCacheImpl(segmentation))
{}

OmSegmentCache::~OmSegmentCache()
{}

OmSegID OmSegmentCache::GetSegmentationID(){
    return segmentation_->GetID();
}

quint32 OmSegmentCache::getPageSize()
{
    return mImpl->getPageSize();
}

OmSegment* OmSegmentCache::AddSegment()
{
    zi::guard g(mutex_);
    return mImpl->AddSegment();
}

OmSegment* OmSegmentCache::AddSegment(OmSegID value)
{
    zi::guard g(mutex_);
    return mImpl->AddSegment(value);
}

OmSegment* OmSegmentCache::GetOrAddSegment(const OmSegID val)
{
    zi::guard g(mutex_);
    return mImpl->GetOrAddSegment(val);
}

bool OmSegmentCache::IsSegmentValid(OmSegID seg)
{
    zi::guard g(mutex_);
    return (NULL != mImpl->GetSegment(seg));
}

OmSegment* OmSegmentCache::GetSegment(const OmSegID value)
{
    zi::guard g(mutex_);
    return mImpl->GetSegment(value);
}

OmSegID OmSegmentCache::GetNumSegments()
{
    zi::guard g(mutex_);
    return mImpl->GetNumSegments();
}

OmSegID OmSegmentCache::GetNumTopSegments()
{
    zi::guard g(mutex_);
    return mImpl->GetNumTopSegments();
}

bool OmSegmentCache::AreSegmentsSelected()
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().AreSegmentsSelected();
}

quint32 OmSegmentCache::numberOfSelectedSegments()
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().numberOfSelectedSegments();
}

const OmSegIDsSet& OmSegmentCache::GetSelectedSegmentIds()
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().GetSelectedSegmentIds();
}

OmSegIDsSet & OmSegmentCache::GetEnabledSegmentIds()
{
    zi::guard g(mutex_);
    return mImpl->EnabledSegments().GetEnabledSegmentIdsRef();
}

bool OmSegmentCache::isSegmentEnabled(OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->EnabledSegments().IsEnabled(segID);
}

bool OmSegmentCache::IsSegmentSelected(OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().isSegmentSelected(segID);
}

bool OmSegmentCache::IsSegmentSelected(OmSegment* seg)
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().isSegmentSelected(seg->value());
}

void OmSegmentCache::setSegmentEnabled(OmSegID segID, bool isEnabled)
{
    zi::guard g(mutex_);
    mImpl->EnabledSegments().SetEnabled(segID, isEnabled);
}

void OmSegmentCache::setSegmentSelected(OmSegID segID,
                                         const bool isSelected,
                                         const bool addToRecentList)
{
    zi::guard g(mutex_);
    mImpl->SegmentSelection().setSegmentSelected(segID, isSelected, addToRecentList);
}

void OmSegmentCache::setSegmentName(OmSegID segID, QString name)
{
    zi::guard g(mutex_);
    mImpl->setSegmentName(segID, name);
}

QString OmSegmentCache::getSegmentName(OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->getSegmentName(segID);
}

void OmSegmentCache::setSegmentNote(OmSegID segID, QString note)
{
    zi::guard g(mutex_);
    mImpl->setSegmentNote(segID, note);
}

QString OmSegmentCache::getSegmentNote(OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->getSegmentNote(segID);
}

OmSegment* OmSegmentCache::findRoot(OmSegment* segment)
{
    zi::guard g(mutex_);
    return mImpl->FindRoot(segment);
}

OmSegment* OmSegmentCache::findRoot(const OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->FindRoot(segID);
}

OmSegID OmSegmentCache::findRootID(const OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->FindRootID(segID);
}

OmSegID OmSegmentCache::findRootID(OmSegment* segment)
{
    zi::guard g(mutex_);
    return mImpl->FindRootID(segment);
}

OmSegIDsSet OmSegmentCache::JoinTheseSegments(const OmSegIDsSet & segmentList)
{
    zi::guard g(mutex_);
    return mImpl->JoinTheseSegments(segmentList);
}

OmSegIDsSet OmSegmentCache::UnJoinTheseSegments(const OmSegIDsSet & segmentList)
{
    zi::guard g(mutex_);
    return mImpl->UnJoinTheseSegments(segmentList);
}

uint32_t OmSegmentCache::getMaxValue()
{
    zi::guard g(mutex_);
    return mImpl->getMaxValue();
}

void OmSegmentCache::UpdateSegmentSelection(const OmSegIDsSet & idsToSelect,
                                             const bool addToRecentList)
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().UpdateSegmentSelection(idsToSelect, addToRecentList);
}

void OmSegmentCache::AddToSegmentSelection(const OmSegIDsSet& idsToSelect)
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().AddToSegmentSelection(idsToSelect);
}

void OmSegmentCache::RemoveFromSegmentSelection(const OmSegIDsSet& idsToSelect)
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().RemoveFromSegmentSelection(idsToSelect);
}

std::pair<bool, OmSegmentEdge> OmSegmentCache::JoinEdge(const OmSegmentEdge& e)
{
    zi::guard g(mutex_);
    return mImpl->JoinFromUserAction(e);
}

OmSegmentEdge OmSegmentCache::SplitEdge(const OmSegmentEdge & e)
{
    zi::guard g(mutex_);
    return mImpl->SplitEdgeUserAction(e);
}

void OmSegmentCache::refreshTree()
{
    zi::guard g(mutex_);
    return mImpl->refreshTree();
}

quint64 OmSegmentCache::getSizeRootAndAllChildren(OmSegment* segUnknownDepth)
{
    zi::guard g(mutex_);
    return mImpl->getSizeRootAndAllChildren(segUnknownDepth);
}

bool OmSegmentCache::AreSegmentsEnabled()
{
    zi::guard g(mutex_);
    return mImpl->EnabledSegments().AnyEnabled();
}

void OmSegmentCache::Flush()
{
    zi::guard g(mutex_);
    return mImpl->Flush();
}

bool OmSegmentCache::AreAnySegmentsInValidList(const OmSegIDsSet& ids)
{
    zi::guard g(mutex_);
    return mImpl->AreAnySegmentsInValidList(ids);
}
