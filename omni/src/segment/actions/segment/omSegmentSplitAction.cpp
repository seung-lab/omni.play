#include "common/omDebug.h"
#include "system/viewGroup/omViewGroupState.h"
#include "datalayer/fs/omActionLoggerFS.h"
#include "segment/actions/segment/omSegmentSplitAction.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"

OmSegmentSplitAction::OmSegmentSplitAction( const SegmentationDataWrapper & sdw,
					    const OmSegmentEdge & edge )
	: mEdge(edge)
	, mSegmentationID(sdw.getSegmentationID())
	, desc("Splitting: ")
{
	SetUndoable(true);
}

void OmSegmentSplitAction::RunIfSplittable( OmSegment * seg1, OmSegment * seg2 )
{
	SegmentationDataWrapper sdw(seg1);
	OmSegmentEdge edge = sdw.getSegmentCache()->findClosestCommonEdge(seg1, seg2);

	if(!edge.isValid()){
		return;
	}

	(new OmSegmentSplitAction(sdw, edge))->OmAction::Run();
}

void OmSegmentSplitAction::Action()
{
	SegmentationDataWrapper sdw(mSegmentationID);
	mEdge = sdw.getSegmentCache()->SplitEdge(mEdge);

	desc = QString("Split seg %1 from %2")
		.arg(mEdge.childID)
		.arg(mEdge.parentID);

	OmEvents::SegmentModified();
}

void OmSegmentSplitAction::UndoAction()
{
	SegmentationDataWrapper sdw(mSegmentationID);
	std::pair<bool, OmSegmentEdge> edge = sdw.getSegmentCache()->JoinEdge(mEdge);

	assert(edge.first && "edge could not be rejoined...");
	mEdge = edge.second;

	desc = QString("Joined seg %1 to %2")
		.arg(mEdge.childID)
		.arg(mEdge.parentID);

	OmEvents::SegmentModified();
}

std::string OmSegmentSplitAction::Description()
{
	return desc.toStdString();
}

void OmSegmentSplitAction::save(const std::string & comment)
{
	OmActionLoggerFS::save(this, comment);
}

void OmSegmentSplitAction::DoFindAndSplitSegment(const SegmentDataWrapper& sdw,
						 OmViewGroupState* vgs)
{
	const std::pair<bool, SegmentDataWrapper> splittingAndSDW =
		vgs->GetSplitMode();
	const bool amAlreadySplitting = splittingAndSDW.first;

        if(amAlreadySplitting){

		SegmentDataWrapper segmentBeingSplit = splittingAndSDW.second;

                OmSegment* seg1 = segmentBeingSplit.getSegment();
                OmSegment* seg2 = sdw.getSegment();

		if(NULL == seg1 || NULL == seg2) {
			return;
		}

		RunIfSplittable(seg1, seg2);

                vgs->SetSplitMode(false);

        } else { // set segment to be split later...
                if(sdw.isValid()){
                        vgs->SetSplitMode(sdw);
                }
        }
}
