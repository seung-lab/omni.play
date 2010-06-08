#include "omSegmentSplitAction.h"
#include "segment/omSegment.h"
#include "volume/omSegmentation.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "system/omEventManager.h"
#include "system/events/omViewEvent.h"

/////////////////////////////////
///////
///////          OmSegmentSplitAction
///////
OmSegmentSplitAction::OmSegmentSplitAction(OmSegment* child, OmId segmentationId)
	: mSegmentationId( segmentationId ),
	  mChild(child)
{
	SetUndoable(true);
}

/////////////////////////////////
///////          Action Methods
void OmSegmentSplitAction::Action()
{
	OmSegmentation& segmentation = OmProject::GetSegmentation(mSegmentationId);
	mParentId = mChild->getParentSegID(); 
	segmentation.GetSegmentCache()->GetCacheImpl()->splitChildFromParent(mChild);
        OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
	
}

void OmSegmentSplitAction::UndoAction()
{
	OmSegmentation & segmentation = OmProject::GetSegmentation(mSegmentationId);
	OmSegIDsSet segIds;
	segIds.insert(mParentId);
	segIds.insert(mChild->getValue());
	segmentation.JoinTheseSegments(segIds);	
             OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));

}

string OmSegmentSplitAction::Description()
{
	QString lineItem = QString("Split: ");


	debug("splitz","child->mParentSegID %i \n",mChild->getParentSegID());	
	lineItem += QString("seg %1 from %2").arg(mChild->getValue()).arg(mChild->getParentSegID());
	return lineItem.toStdString();
}
