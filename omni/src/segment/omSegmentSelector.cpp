#include "actions/omActions.h"
#include "actions/omSelectSegmentParams.hpp"
#include "common/omDebug.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

OmSegmentSelector::OmSegmentSelector(const SegmentationDataWrapper& sdw,
                                     void* sender,
                                     const std::string& cmt )
    : segmentCache_(sdw.SegmentCache())
    , params_(boost::make_shared<OmSelectSegmentsParams>())
{
    params_->sdw = SegmentDataWrapper(sdw, 0);
    params_->sender = sender;
    params_->comment = cmt;
    params_->oldSelectedIDs = segmentCache_->GetSelectedSegmentIds();
    params_->newSelectedIDs = params_->oldSelectedIDs;
    params_->autoCenter = false;
    params_->shouldScroll = true;
    params_->addToRecentList = true;

    params_->augmentListOnly = false;
    params_->addOrSubtract = om::ADD;
}

void OmSegmentSelector::selectNoSegments()
{
    params_->newSelectedIDs.clear();
}

void OmSegmentSelector::selectJustThisSegment( const OmSegID segIDunknownLevel,
                                               const bool isSelected)
{
    selectNoSegments();

    const OmSegID segID = segmentCache_->findRootID( segIDunknownLevel );
    if(!segID){
        return;
    }

    if( params_->oldSelectedIDs.size() > 1 ){
        params_->newSelectedIDs.insert( segID );
    } else {
        if( isSelected ){
            params_->newSelectedIDs.insert( segID );
        }
    }

    setSelectedSegment(segID);
}

void OmSegmentSelector::setSelectedSegment(const OmSegID segID)
{
    params_->sdw.SetSegmentID(segID);
    OmSegmentSelected::Set(params_->sdw);
}

void OmSegmentSelector::InsertSegments(const boost::unordered_set<OmSegID>& segIDs)
{
    FOR_EACH(iter, segIDs){
        params_->newSelectedIDs.insert(segmentCache_->findRootID(*iter));
    }
}

void OmSegmentSelector::RemoveSegments(const boost::unordered_set<OmSegID>& segIDs)
{
    params_->newSelectedIDs.clear();

    FOR_EACH(iter, segIDs){
        params_->newSelectedIDs.insert(segmentCache_->findRootID(*iter));
    }
}

void OmSegmentSelector::augmentSelectedSet( const OmSegID segIDunknownLevel,
                                            const bool isSelected)
{
    const OmSegID segID = segmentCache_->findRootID( segIDunknownLevel );

    if(!segID){
        return;
    }

    if(isSelected) {
        params_->newSelectedIDs.insert(segID);
    } else {
        params_->newSelectedIDs.erase(segID);
    }

    setSelectedSegment(segID);
}

void OmSegmentSelector::selectJustThisSegment_toggle(const OmSegID segIDunknownLevel)
{
    const OmSegID segID = segmentCache_->findRootID( segIDunknownLevel );
    if(!segID){
        return;
    }

    const bool isSelected = segmentCache_->IsSegmentSelected( segID );
    selectJustThisSegment( segID, !isSelected );
}

void OmSegmentSelector::augmentSelectedSet_toggle(const OmSegID segIDunknownLevel)
{
    const OmSegID segID = segmentCache_->findRootID( segIDunknownLevel );
    if(!segID){
        return;
    }

    const bool isSelected = segmentCache_->IsSegmentSelected( segID );
    augmentSelectedSet( segID, !isSelected );
}

bool OmSegmentSelector::sendEvent()
{
    if( params_->oldSelectedIDs == params_->newSelectedIDs ){
        debug(segmentSelector, "not sending segment list\n");
        return false;
    }

    //debugs(segmentSelector) << params_->oldSelectedIDs << "\n";

    OmActions::SelectSegments(params_);

    return true;
}

void OmSegmentSelector::ShouldScroll(const bool shouldScroll){
    params_->shouldScroll = shouldScroll;
}

void OmSegmentSelector::AddToRecentList(const bool addToRecentList){
    params_->addToRecentList = addToRecentList;
}

void OmSegmentSelector::AutoCenter(const bool autoCenter){
    params_->autoCenter = autoCenter;
}

void OmSegmentSelector::AugmentListOnly(const bool augmentListOnly){
    params_->augmentListOnly = augmentListOnly;
}

void OmSegmentSelector::AddOrSubtract(const om::AddOrSubtract addOrSubtract){
    params_->addOrSubtract = addOrSubtract;
}
