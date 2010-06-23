#include "common/omDebug.h"
#include "gui/inspectors/segObjectInspector.h"
#include "omSegmentContextMenu.h"
#include "project/omProject.h"
#include "segment/actions/edit/omEditSelectionSetAction.h"
#include "segment/actions/segment/omSegmentJoinAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/voxel/omVoxelSetConnectedAction.h"
#include "segment/omSegmentSelector.h"
#include "system/events/omViewEvent.h"
#include "system/omCacheManager.h"
#include "system/omEventManager.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

/////////////////////////////////
///////          Context Menu Methods

void OmSegmentContextMenu::Refresh( SegmentDataWrapper sdw, OmViewGroupState * vgs)
{
	mSegmentationId = sdw.getSegmentationID();
	mSegmentId = sdw.getID();
	mViewGroupState = vgs;

	//clear old menu actions
	clear();

	AddSelectionNames();
	addSeparator();

	AddSelectionAction();
        addSeparator();

        AddColorActions();
	addSeparator();

	AddDendActions();
	addSeparator();

	AddGroupActions();
	addSeparator();

	AddPropertiesActions();
}

void OmSegmentContextMenu::AddSelectionNames()
{
	//get segmentation and segment
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);
	OmSegment * r_segment = r_segmentation.GetSegment(mSegmentId);

	addAction( QString("Segment ") + QString::number(r_segment->getValue()) +
			QString(" (Root ") + QString::number(r_segment->getRootSegID()) + QString(")"));
	addAction( r_segmentation.GetName() );
}

/*
 *	Adds Un/Select Segment Action
 */
void OmSegmentContextMenu::AddSelectionAction()
{
	//get segmentation and segment
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

	//is segment selected
	bool is_segment_selected = r_segmentation.IsSegmentSelected(mSegmentId);

	//if segment is already selected
	if (is_segment_selected) {
		addAction(QString("Select Only This Segment"), this, SLOT(UnselectOthers()));
		addAction(QString("Deselect Only This Segment"), this, SLOT(Unselect()));
	} else {
		addAction(QString("Select Segment"), this, SLOT(Select()));
		addAction(QString("Select Only This Segment"), this, SLOT(UnselectOthers()));
	}
}

void OmSegmentContextMenu::AddEditSelectionAction()
{
	addAction(QString("Set Edit Selection"), this, SLOT(SetEditSelection()));
}

/*
 *	Merge Segments
 */
void OmSegmentContextMenu::AddDendActions()
{
	addAction(QString("Merge Selected Segments"), this, SLOT(MergeSegments()));
	addAction(QString("Split Segments"), this, SLOT(splitSegments()));
}

/*
 *	Merge Segments
 */
void OmSegmentContextMenu::AddVoxelAction()
{
	addAction(QString("Set Connected Voxels"), this, SLOT(SetConnectedVoxels()));
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

void OmSegmentContextMenu::SetEditSelection()
{
	(new OmEditSelectionSetAction(mSegmentationId, mSegmentId))->Run();
}

void OmSegmentContextMenu::MergeSegments()
{
        OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
        OmIDsSet ids = seg.GetSelectedSegmentIds();
	(new OmSegmentJoinAction(mSegmentationId, ids))->Run();
}

void OmSegmentContextMenu::splitSegments()
{
	mViewGroupState->SetShowSplitMode(true);
        OmStateManager::SetSystemMode(DEND_MODE);
	mViewGroupState->SetSplitMode(mSegmentationId, mSegmentId);
}

void OmSegmentContextMenu::SetConnectedVoxels()
{
	(new OmVoxelSetConnectedAction())->Run();
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
        OmSegment * r_segment = r_segmentation.GetSegment(r_segmentation.GetSegment(mSegmentId)->getRootSegID());

	r_segment->reRandomizeColor();
	OmCacheManager::Freshen(true);
	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
}

void OmSegmentContextMenu::addGroup()
{
        debug("validate", "OmSegmentContextMenu::addGroup\n");
        if (OmProject::IsSegmentationValid(mSegmentationId)) {
                OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
                OmSegIDsSet set;
		set.insert(seg.GetSegment(seg.GetSegment(mSegmentId)->getRootSegID())->getValue());
                seg.SetGroup(set, VALIDROOT, QString("Valid"));
        }
}

void OmSegmentContextMenu::deleteGroup()
{
        debug("validate", "OmSegmentContextMenu::addGroup\n");
        if (OmProject::IsSegmentationValid(mSegmentationId)) {
                OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
                OmSegIDsSet set;
                set.insert(seg.GetSegment(seg.GetSegment(mSegmentId)->getRootSegID())->getValue());
                seg.SetGroup(set, NOTVALIDROOT, QString("Not Valid"));
        }
}

void OmSegmentContextMenu::showProperties()
{
        OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	OmSegID segid;

	segid = seg.GetSegment(mSegmentId)->getRootSegID();

	mViewGroupState->GetInspectorProperties()->setOrReplaceWidget( new SegObjectInspector(
						 SegmentDataWrapper(mSegmentationId, segid), this),
                                                 QString("Segmentation%1: Segment %2")
                                                 .arg(mSegmentationId)
                                                 .arg(segid) );
}

void OmSegmentContextMenu::AddPropertiesActions()
{
        addAction(QString("Properties"), this, SLOT(showProperties()));
}
