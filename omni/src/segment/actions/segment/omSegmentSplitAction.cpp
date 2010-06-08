#include "omSegmentSplitAction.h"
#include "segment/omSegment.h"
#include "volume/omSegmentation.h"
#include "project/omProject.h"
#include "segment/omSegmentEdge.h"
#include "system/omEventManager.h"
#include "system/events/omViewEvent.h"
#include "system/events/omSegmentEvent.h"

/////////////////////////////////
///////
///////          OmSegmentSplitAction
///////
OmSegmentSplitAction::OmSegmentSplitAction(OmSegment * seg1, OmSegment * seg2 )
	: mSeg1(seg1)
	, mSeg2(seg2)
	, edge(NULL)
	, desc("Splitting: ")
{
	SetUndoable(true);
}

/////////////////////////////////
///////          Action Methods
void OmSegmentSplitAction::Action()
{
	edge = mSeg1->splitTwoChildren(mSeg2);
	if( NULL == edge ){
		desc = "Segments not in same tree";
	} else {
		desc = QString("Split seg %1 from %2")
			.arg(edge->childID)
			.arg(edge->parentID);
		OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
	}
}

void OmSegmentSplitAction::UndoAction()
{
	if( NULL == edge ){
		return;
	}

	OmSegmentation & segmentation = OmProject::GetSegmentation( mSeg1->getSegmentationID() );
	edge = segmentation.JoinEdge( edge );

	desc = QString("Joined seg %1 to %2")
		.arg(edge->childID)
		.arg(edge->parentID);

	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
}

string OmSegmentSplitAction::Description()
{
	return desc.toStdString();
}
