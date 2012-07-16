#include "actions/omActions.h"
#include "actions/omSelectSegmentParams.hpp"
#include "common/omDebug.h"
#include "common/omSet.hpp"
#include "project/omProject.h"
#include "segment/omSegments.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegmentsImpl.h"

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

void OmSegmentSelector::setOrderOfAdditionToZero(OmSegID segID)
{
    OmSegmentsImpl *impl_ = (*segments_).GetImpl();
    OmSegmentGraph *segmentGraph_ = (*impl_).GetGraph();
    boost::unordered_map < OmSegID, std::vector <OmMSTEdge*> > *adjacencyList_ = (*segmentGraph_).GetAdjacencyList();

    for ( uint32_t i=0; i<(*adjacencyList_)[segID].size(); i++ )
        (* (*adjacencyList_)[segID][i] ).orderOfAddition = 0;
}

void OmSegmentSelector::setOrderOfAdditionToNextNumber(OmSegID segID)
{
    OmSegID otherSeg;
    OmSegmentsImpl *impl_ = (*segments_).GetImpl();
    OmSegmentGraph *segmentGraph_ = (*impl_).GetGraph();
    boost::unordered_map < OmSegID, std::vector <OmMSTEdge*> > *adjacencyList_ = (*segmentGraph_).GetAdjacencyList();

    for ( uint32_t i=0; i<(*adjacencyList_)[segID].size(); i++ )
    {
        if ( (* (*adjacencyList_)[segID][i] ).orderOfAddition ) continue;

        otherSeg = (* (*adjacencyList_)[segID][i] ).node1ID;
        if ( otherSeg == segID ) otherSeg = (* (*adjacencyList_)[segID][i] ).node2ID;
        if ( !IsSegmentSelected(otherSeg) ) continue;
        
        numberOfAddedSegment++;
        (* (*adjacencyList_)[segID][i] ).orderOfAddition = numberOfAddedSegment;
    }
}

void OmSegmentSelector::selectNoSegments()
{
    FOR_EACH(iter, params_->newSelectedIDs)
    {
        setOrderOfAdditionToZero(*iter);
    }

    params_->newSelectedIDs.clear();
    numberOfAddedSegment = 0;
}

void OmSegmentSelector::selectJustThisSegment( const OmSegID segIDunknownLevel,
                                               const bool isSelected)
{
    selectNoSegments();

    const OmSegID segID = segments_->findRootID( segIDunknownLevel );
    if(!segID){
        return;
    }

    if( params_->oldSelectedIDs.size() > 1 ){
        params_->newSelectedIDs.insert( segID );
        setOrderOfAdditionToNextNumber( segID );

    } else {
        if( isSelected ){
            params_->newSelectedIDs.insert( segID );
            setOrderOfAdditionToNextNumber( segID );
        }
    }

    setSelectedSegment(segID);
}

void OmSegmentSelector::setSelectedSegment(const OmSegID segID)
{
    params_->sdw.SetSegmentID(segID);
    OmSegmentSelected::Set(params_->sdw);
}

void OmSegmentSelector::InsertSegments(const boost::unordered_set<OmSegID>* segIDs)
{
    uint32_t segID;
    FOR_EACH(iter, *segIDs){
        segID = segments_->findRootID(*iter);
        params_->newSelectedIDs.insert( segID );
        setOrderOfAdditionToNextNumber( segID );
    }
}

void OmSegmentSelector::RemoveSegments(const boost::unordered_set<OmSegID>* segIDs)
{
    params_->newSelectedIDs.clear();

    uint32_t segID;
    FOR_EACH(iter, *segIDs){
        segID = segments_->findRootID(*iter);
        params_->newSelectedIDs.insert(segID);
        setOrderOfAdditionToNextNumber(segID);
    }
}

void OmSegmentSelector::augmentSelectedSet( const OmSegID segIDunknownLevel,
                                            const bool isSelected)
{
    const OmSegID segID = segments_->findRootID( segIDunknownLevel );

    if(!segID){
        return;
    }

    if(isSelected) {
        params_->newSelectedIDs.insert(segID);
        setOrderOfAdditionToNextNumber(segID);
    } else {
        params_->newSelectedIDs.erase(segID);
        setOrderOfAdditionToZero(segID);
    }

    setSelectedSegment(segID);
}

void OmSegmentSelector::selectJustThisSegment_toggle(const OmSegID segIDunknownLevel)
{
    const OmSegID segID = segments_->findRootID( segIDunknownLevel );
    if(!segID){
        return;
    }

    const bool isSelected = segments_->IsSegmentSelected( segID );
    selectJustThisSegment( segID, !isSelected );
}

void OmSegmentSelector::augmentSelectedSet_toggle(const OmSegID segIDunknownLevel)
{
    const OmSegID segID = segments_->findRootID( segIDunknownLevel );
    if(!segID){
        return;
    }

    const bool isSelected = segments_->IsSegmentSelected( segID );
    augmentSelectedSet( segID, !isSelected );
}

bool OmSegmentSelector::IsSegmentSelected(const OmSegID segID)
{
    return segments_->IsSegmentSelected( segID );
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
        OmSegments* segments = params_->sdw.Segments();

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
