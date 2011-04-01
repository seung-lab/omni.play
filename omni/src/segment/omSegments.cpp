#include "datalayer/omDataPath.h"
#include "datalayer/omDataPath.h"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "volume/omSegmentation.h"

OmSegments::OmSegments(OmSegmentation* segmentation)
    : segmentation_(segmentation)
    , mImpl(new OmSegmentsImpl(segmentation))
{}

OmSegments::~OmSegments()
{}

OmSegID OmSegments::GetSegmentationID(){
    return segmentation_->GetID();
}

uint32_t OmSegments::getPageSize()
{
    zi::guard g(mutex_);
    return mImpl->getPageSize();
}

OmSegment* OmSegments::AddSegment()
{
    zi::guard g(mutex_);
    return mImpl->AddSegment();
}

OmSegment* OmSegments::AddSegment(OmSegID value)
{
    zi::guard g(mutex_);
    return mImpl->AddSegment(value);
}

OmSegment* OmSegments::GetOrAddSegment(const OmSegID val)
{
    zi::guard g(mutex_);
    return mImpl->GetOrAddSegment(val);
}

bool OmSegments::IsSegmentValid(OmSegID seg)
{
    zi::guard g(mutex_);
    return (NULL != mImpl->GetSegment(seg));
}

OmSegment* OmSegments::GetSegment(const OmSegID value)
{
    zi::guard g(mutex_);
    return mImpl->GetSegment(value);
}

OmSegID OmSegments::GetNumSegments()
{
    zi::guard g(mutex_);
    return mImpl->GetNumSegments();
}

OmSegID OmSegments::GetNumTopSegments()
{
    zi::guard g(mutex_);
    return mImpl->GetNumTopSegments();
}

bool OmSegments::AreSegmentsSelected()
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().AreSegmentsSelected();
}

quint32 OmSegments::numberOfSelectedSegments()
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().numberOfSelectedSegments();
}

const OmSegIDsSet& OmSegments::GetSelectedSegmentIds()
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().GetSelectedSegmentIds();
}

OmSegIDsSet & OmSegments::GetEnabledSegmentIds()
{
    zi::guard g(mutex_);
    return mImpl->EnabledSegments().GetEnabledSegmentIdsRef();
}

bool OmSegments::isSegmentEnabled(OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->EnabledSegments().IsEnabled(segID);
}

bool OmSegments::IsSegmentSelected(OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().isSegmentSelected(segID);
}

bool OmSegments::IsSegmentSelected(OmSegment* seg)
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().isSegmentSelected(seg->value());
}

void OmSegments::setSegmentEnabled(OmSegID segID, bool isEnabled)
{
    zi::guard g(mutex_);
    mImpl->EnabledSegments().SetEnabled(segID, isEnabled);
}

void OmSegments::setSegmentSelected(OmSegID segID,
                                         const bool isSelected,
                                         const bool addToRecentList)
{
    zi::guard g(mutex_);
    mImpl->SegmentSelection().setSegmentSelected(segID, isSelected, addToRecentList);
}

void OmSegments::setSegmentName(OmSegID segID, QString name)
{
    zi::guard g(mutex_);
    mImpl->setSegmentName(segID, name);
}

QString OmSegments::getSegmentName(OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->getSegmentName(segID);
}

void OmSegments::setSegmentNote(OmSegID segID, QString note)
{
    zi::guard g(mutex_);
    mImpl->setSegmentNote(segID, note);
}

QString OmSegments::getSegmentNote(OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->getSegmentNote(segID);
}

OmSegment* OmSegments::findRoot(OmSegment* segment)
{
    zi::guard g(mutex_);
    return mImpl->FindRoot(segment);
}

OmSegment* OmSegments::findRoot(const OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->FindRoot(segID);
}

OmSegID OmSegments::findRootID(const OmSegID segID)
{
    zi::guard g(mutex_);
    return mImpl->FindRootID(segID);
}

OmSegID OmSegments::findRootID(OmSegment* segment)
{
    zi::guard g(mutex_);
    return mImpl->FindRootID(segment);
}

OmSegIDsSet OmSegments::JoinTheseSegments(const OmSegIDsSet & segmentList)
{
    zi::guard g(mutex_);
    return mImpl->JoinTheseSegments(segmentList);
}

OmSegIDsSet OmSegments::UnJoinTheseSegments(const OmSegIDsSet & segmentList)
{
    zi::guard g(mutex_);
    return mImpl->UnJoinTheseSegments(segmentList);
}

uint32_t OmSegments::getMaxValue()
{
    // self-locking integer
    return mImpl->getMaxValue();
}

void OmSegments::UpdateSegmentSelection(const OmSegIDsSet & idsToSelect,
                                             const bool addToRecentList)
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().UpdateSegmentSelection(idsToSelect, addToRecentList);
}

void OmSegments::AddToSegmentSelection(const OmSegIDsSet& idsToSelect)
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().AddToSegmentSelection(idsToSelect);
}

void OmSegments::RemoveFromSegmentSelection(const OmSegIDsSet& idsToSelect)
{
    zi::guard g(mutex_);
    return mImpl->SegmentSelection().RemoveFromSegmentSelection(idsToSelect);
}

std::pair<bool, OmSegmentEdge> OmSegments::JoinEdge(const OmSegmentEdge& e)
{
    zi::guard g(mutex_);
    return mImpl->JoinFromUserAction(e);
}

OmSegmentEdge OmSegments::SplitEdge(const OmSegmentEdge & e)
{
    zi::guard g(mutex_);
    return mImpl->SplitEdgeUserAction(e);
}

void OmSegments::refreshTree()
{
    zi::guard g(mutex_);
    return mImpl->refreshTree();
}

bool OmSegments::AreSegmentsEnabled()
{
    zi::guard g(mutex_);
    return mImpl->EnabledSegments().AnyEnabled();
}

void OmSegments::Flush()
{
    zi::guard g(mutex_);
    return mImpl->Flush();
}

bool OmSegments::AreAnySegmentsInValidList(const OmSegIDsSet& ids)
{
    zi::guard g(mutex_);
    return mImpl->AreAnySegmentsInValidList(ids);
}

uint64_t OmSegments::MSTfreshness() const
{
    zi::guard g(mutex_);
    return mImpl->MSTfreshness();
}

OmSegmentChildren* OmSegments::Children()
{
    // no locking
    return mImpl->Children();
}
