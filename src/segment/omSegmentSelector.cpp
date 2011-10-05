#include "actions/omActions.h"
#include "actions/omSelectSegmentParams.hpp"
#include "common/omDebug.h"
#include "common/omSet.hpp"
#include "project/project.h"
#include "segment/segments.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "utility/dataWrappers.h"
#include "volume/segmentation.h"

OmSegmentSelector::OmSegmentSelector(const SegmentationDataWrapper& sdw,
                                     void* sender,
                                     const std::string& cmt )
    : segments_(sdw.Segments())
    , params_(om::make_shared<OmSelectSegmentsParams>())
{
    params_->sdw = SegmentDataWrapper(sdw, 0);
    params_->sender = sender;
    params_->comment = cmt;
    params_->oldSelectedIDs = segments_->GetSelectedSegmentIDs();
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

void OmSegmentSelector::selectJustThisSegment( const segId segIDunknownLevel,
                                               const bool isSelected)
{
    selectNoSegments();

    const segId segID = segments_->findRootID( segIDunknownLevel );
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

void OmSegmentSelector::setSelectedSegment(const segId segID)
{
    params_->sdw.SetSegmentID(segID);
    OmSegmentSelected::Set(params_->sdw);
}

void OmSegmentSelector::InsertSegments(const boost::unordered_set<segId>* segIDs)
{
    FOR_EACH(iter, *segIDs){
        params_->newSelectedIDs.insert(segments_->findRootID(*iter));
    }
}

void OmSegmentSelector::RemoveSegments(const boost::unordered_set<segId>* segIDs)
{
    params_->newSelectedIDs.clear();

    FOR_EACH(iter, *segIDs){
        params_->newSelectedIDs.insert(segments_->findRootID(*iter));
    }
}

void OmSegmentSelector::augmentSelectedSet( const segId segIDunknownLevel,
                                            const bool isSelected)
{
    const segId segID = segments_->findRootID( segIDunknownLevel );

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

void OmSegmentSelector::selectJustThisSegment_toggle(const segId segIDunknownLevel)
{
    const segId segID = segments_->findRootID( segIDunknownLevel );
    if(!segID){
        return;
    }

    const bool isSelected = segments_->IsSegmentSelected( segID );
    selectJustThisSegment( segID, !isSelected );
}

void OmSegmentSelector::augmentSelectedSet_toggle(const segId segIDunknownLevel)
{
    const segId segID = segments_->findRootID( segIDunknownLevel );
    if(!segID){
        return;
    }

    const bool isSelected = segments_->IsSegmentSelected( segID );
    augmentSelectedSet( segID, !isSelected );
}


bool OmSegmentSelector::sendEvent()
{
    if(params_->augmentListOnly)
    {
        if(om::ADD == params_->addOrSubtract){
            if(om::set::SetAContainsB(params_->oldSelectedIDs,
                                      params_->newSelectedIDs))
            {
                // already added
                return false;
            }
        } else {
            if(om::set::SetsAreDisjoint(params_->oldSelectedIDs,
                                        params_->newSelectedIDs))
            {
                // no segments to be removed are selected
                return false;
            }
        }

    } else {
        if( params_->oldSelectedIDs == params_->newSelectedIDs ){
            // no change in selected set
            return false;
        }
    }

    //debugs(segmentSelector) << params_->oldSelectedIDs << "\n";

    if(params_->augmentListOnly){
        // disable undo option for now
        segments* segments = params_->sdw.Segments();

        if(om::ADD == params_->addOrSubtract){
            segments->AddToSegmentSelection(params_->newSelectedIDs);
        } else {
            segments->RemoveFromSegmentSelection(params_->newSelectedIDs);
        }
    } else {
        OmActions::SelectSegments(params_);
    }

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
