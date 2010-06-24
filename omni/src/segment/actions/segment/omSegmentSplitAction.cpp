#include "project/omProject.h"
#include "segment/actions/segment/omSegmentSplitAction.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "system/omEventManager.h"
#include "volume/omSegmentation.h"

OmSegmentSplitAction::OmSegmentSplitAction( const SegmentationDataWrapper & sdw, 
					    const OmSegmentEdge & edge )
	: mEdge(edge)
	, m_sdw(sdw)
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
	mEdge = m_sdw.getSegmentCache()->SplitEdge(mEdge);

	desc = QString("Split seg %1 from %2")
		.arg(mEdge.childID)
		.arg(mEdge.parentID);
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
}

void OmSegmentSplitAction::UndoAction()
{
	mEdge = m_sdw.getSegmentCache()->JoinEdge(mEdge);

	desc = QString("Joined seg %1 to %2")
		.arg(mEdge.childID)
		.arg(mEdge.parentID);

	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
}

string OmSegmentSplitAction::Description()
{
	return desc.toStdString();
}
