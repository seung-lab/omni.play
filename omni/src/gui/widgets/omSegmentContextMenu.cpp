#include "segment/omSegmentIterator.h"
#include "common/omDebug.h"
#include "gui/inspectors/segObjectInspector.h"
#include "omSegmentContextMenu.h"
#include "project/omProject.h"
#include "segment/actions/segment/omSegmentJoinAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"
#include "system/cache/omCacheManager.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "system/omEvents.h"

/////////////////////////////////
///////          Context Menu Methods

void OmSegmentContextMenu::Refresh( SegmentDataWrapper sdw, OmViewGroupState * vgs)
{
	mSegmentationId = sdw.getSegmentationID();
        OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	mSegmentId = sdw.getID();
	mViewGroupState = vgs;
	OmSegID rootId = seg.GetSegmentCache()->findRootID(mSegmentId);
	mImmutable = seg.GetSegmentCache()->GetSegment(rootId)->GetImmutable();

	//clear old menu actions
	clear();

	AddSelectionNames();
	addSeparator();

	AddSelectionAction();
        addSeparator();

        AddColorActions();
	addSeparator();

	if(!mImmutable) {
		AddDendActions();
		addSeparator();
	}

	AddGroupActions();
	addSeparator();

	AddPropertiesActions();
}

void OmSegmentContextMenu::AddSelectionNames()
{
	//get segmentation and segment
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);
	OmSegment * r_segment = r_segmentation.GetSegmentCache()->GetSegment(mSegmentId);

	addAction( "Segment " + QString::number(r_segment->value)
		   + " (Root " + QString::number(r_segment->getRootSegID())
		   + ")" );
	QString validText;
	if(mImmutable) {
		validText = "Valid in ";
	} else {
		validText = "Not valid in ";
	}
	addAction( validText + QString::fromStdString(r_segmentation.GetName()));
}

/*
 *	Adds Un/Select Segment Action
 */
void OmSegmentContextMenu::AddSelectionAction()
{
	//get segmentation and segment
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

	//is segment selected
	bool is_segment_selected = r_segmentation.GetSegmentCache()->IsSegmentSelected(mSegmentId);

	//if segment is already selected
	if (is_segment_selected) {
		addAction(QString("Select Only This Segment"), this, SLOT(UnselectOthers()));
		addAction(QString("Deselect Only This Segment"), this, SLOT(Unselect()));
	} else {
		addAction(QString("Select Only This Segment"), this, SLOT(UnselectOthers()));
		addAction(QString("Select Segment"), this, SLOT(Select()));
	}
}

/*
 *	Merge Segments
 */
void OmSegmentContextMenu::AddDendActions()
{
	addAction(QString("Merge Selected Segments"), this, SLOT(MergeSegments()));
	addAction(QString("Split Segments"), this, SLOT(splitSegments()));
}


/////////////////////////////////
///////          Context Menu Slots Methods

void OmSegmentContextMenu::Select()
{
	OmSegmentSelector sel(mSegmentationId, this, "view3d" );
	sel.augmentSelectedSet( mSegmentId, true);
	sel.sendEvent();
}

void OmSegmentContextMenu::Unselect()
{
	OmSegmentSelector sel(mSegmentationId, this, "view3d" );
	sel.augmentSelectedSet(mSegmentId, false);
	sel.sendEvent();
}

void OmSegmentContextMenu::UnselectOthers()
{
	OmSegmentSelector sel(mSegmentationId, this, "view3d" );
	sel.selectNoSegments();
	sel.selectJustThisSegment(mSegmentId, true);
	sel.sendEvent();
}

void OmSegmentContextMenu::MergeSegments()
{
        OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
        OmSegIDsSet ids = seg.GetSegmentCache()->GetSelectedSegmentIds();
	(new OmSegmentJoinAction(mSegmentationId, ids))->Run();
}

void OmSegmentContextMenu::splitSegments()
{
	mViewGroupState->SetShowSplitMode(true);
	OmStateManager::SetToolModeAndSendEvent(SPLIT_MODE);
	mViewGroupState->SetSplitMode(mSegmentationId, mSegmentId);
}

void OmSegmentContextMenu::AddColorActions()
{
        addAction(QString("Randomize Segment Color"), this, SLOT(randomizeColor()));
}

void OmSegmentContextMenu::AddGroupActions()
{
        addAction(QString("Set Segment Valid"), this, SLOT(addGroup()));
        addAction(QString("Set Segment Not Valid"), this, SLOT(deleteGroup()));
}

void OmSegmentContextMenu::randomizeColor()
{
        OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);
        OmSegment * r_segment = r_segmentation.GetSegmentCache()->findRoot(mSegmentId);

	r_segment->reRandomizeColor();
	OmCacheManager::TouchFresheness();
	OmEvents::Redraw();
}

void OmSegmentContextMenu::addGroup()
{
        //debug(validate, "OmSegmentContextMenu::addGroup\n");
        if (OmProject::IsSegmentationValid(mSegmentationId)) {
                OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
                OmSegIDsSet set;
		set.insert(seg.GetSegmentCache()->findRootID(mSegmentId));
                seg.SetGroup(set, VALIDROOT, QString("Valid"));
                OmEvents::SegmentModified();
        }
}

void OmSegmentContextMenu::deleteGroup()
{
        //debug(validate, "OmSegmentContextMenu::addGroup\n");
        if (OmProject::IsSegmentationValid(mSegmentationId)) {
                OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
                OmSegIDsSet set;
                set.insert(seg.GetSegmentCache()->findRootID(mSegmentId));
                seg.SetGroup(set, NOTVALIDROOT, QString("Not Valid"));
                OmEvents::SegmentModified();
        }
}

void OmSegmentContextMenu::showProperties()
{
        OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	OmSegID segid;

	segid = seg.GetSegmentCache()->findRootID(mSegmentId);

	mViewGroupState->GetInspectorProperties()->setOrReplaceWidget( new SegObjectInspector(
						 SegmentDataWrapper(mSegmentationId, segid), this),
                                                 QString("Segmentation%1: Segment %2")
                                                 .arg(mSegmentationId)
                                                 .arg(segid) );
}

void OmSegmentContextMenu::AddPropertiesActions()
{
        addAction(QString("Properties"), this, SLOT(showProperties()));
	addAction(QString("List Children"), this, SLOT(printChildren()));
}

void OmSegmentContextMenu::printChildren()
{
	//debug(validate, "OmSegmentContextMenu::addGroup\n");
	if (OmProject::IsSegmentationValid(mSegmentationId)) {
		OmSegmentation & segmentation = OmProject::GetSegmentation(mSegmentationId);
		OmSegmentIterator iter(segmentation.GetSegmentCache());
		iter.iterOverSegmentID(segmentation.GetSegmentCache()->findRoot(mSegmentId)->value);

		OmSegment * seg = iter.getNextSegment();
		while(NULL != seg) {
			printf("%u : %u, %f, %llu\n",
			       seg->value,
			       seg->getParentSegID(),
			       seg->getThreshold(),
			       seg->getSize());
			seg = iter.getNextSegment();
		}

	}

}
