#include "common/omDebug.h"
#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentSplitAction.h"
#include "actions/details/omSegmentSplitActionImpl.hpp"
#include "segment/omFindCommonEdge.hpp"

OmSegmentSplitAction::OmSegmentSplitAction( const SegmentationDataWrapper & sdw,
											const OmSegmentEdge & edge )
	: impl_(boost::make_shared<OmSegmentSplitActionImpl>(sdw, edge))
{
	SetUndoable(true);
}

OmSegmentSplitAction::OmSegmentSplitAction( const SegmentDataWrapper & sdw, const DataCoord coord1, const DataCoord coord2)
	: impl_(boost::make_shared<OmSegmentSplitActionImpl>(sdw, coord1, coord2))
{
	SetUndoable(true);
}

void OmSegmentSplitAction::RunIfSplittable( OmSegment * seg1, OmSegment * seg2, const DataCoord coord1, const DataCoord coord2 )
{
	SegmentationDataWrapper sdw(seg1);
	if(seg1 == seg2) {
		(new OmSegmentSplitAction(SegmentDataWrapper(seg1), coord1, coord2))->Run();
		return;
	}

	OmSegmentEdge edge =
		OmFindCommonEdge::FindClosestCommonEdge(sdw.SegmentCache(),
												seg1,
												seg2);
	if(!edge.isValid()){
		printf("edge was not splittable\n");
		return;
	}

	(new OmSegmentSplitAction(sdw, edge))->Run();
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
	OmActionLogger::save(impl_, comment);
}

void OmSegmentSplitAction::DoFindAndCutSegment(const SegmentDataWrapper& sdw,
												 OmViewGroupState* vgs)
{
	OmSegment* seg1 = sdw.getSegment();
	OmSegment* seg2 = seg1->getParent();

	RunIfSplittable(seg1, seg2, DataCoord(), DataCoord());

	vgs->SetCutMode(false);
}
