#include "common/omDebug.h"
#include "datalayer/fs/omActionLoggerFS.h"
#include "actions/omSegmentSplitAction.h"
#include "actions/omSegmentSplitActionImpl.hpp"

OmSegmentSplitAction::OmSegmentSplitAction( const SegmentationDataWrapper & sdw,
											const OmSegmentEdge & edge )
	: impl_(boost::make_shared<OmSegmentSplitActionImpl>(sdw, edge))
{
	SetUndoable(true);
}

void OmSegmentSplitAction::RunIfSplittable( OmSegment * seg1, OmSegment * seg2 )
{
	SegmentationDataWrapper sdw(seg1);
	OmSegmentEdge edge = sdw.getSegmentCache()->findClosestCommonEdge(seg1, seg2);

	if(!edge.isValid()){
		printf("edge was not splittable\n");
		return;
	}

	(new OmSegmentSplitAction(sdw, edge))->OmAction::Run();
}

void OmSegmentSplitAction::Action()
{
	impl_->Execute();
}

void OmSegmentSplitAction::UndoAction()
{
	impl_->Undo();
}

std::string OmSegmentSplitAction::Description()
{
	return impl_->Description();
}

void OmSegmentSplitAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(impl_, comment);
}

//TODO: put this somewhere else...
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
		if(sdw.isValidWrapper()){
			vgs->SetSplitMode(sdw);
		}
	}
}
