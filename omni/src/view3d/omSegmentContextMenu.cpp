#include "common/omDebug.h"
#include "omSegmentContextMenu.h"
#include "project/omProject.h"
#include "segment/actions/edit/omEditSelectionSetAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/segment/omSegmentJoinAction.h"
#include "segment/actions/voxel/omVoxelSetConnectedAction.h"
#include "segment/omSegmentSelector.h"
#include "utility/setUtilities.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "system/viewGroup/omViewGroupState.h"
#include "system/omStateManager.h"

/////////////////////////////////
///////          Context Menu Methods

void OmSegmentContextMenu::Refresh(OmId segmentationId, OmId segmentId, OmViewGroupState * vgs)
{

	//store ids
	mSegmentationId = segmentationId;
	mSegmentId = segmentId;
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
	} else {
		addAction(QString("Select Segment"), this, SLOT(Select()));
	}

	//	addAction(QString("Select All"));
}

/*
 *	Adds Disable Segment Actions
 */
void OmSegmentContextMenu::AddDisableActions()
{

	//is segment selected
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);
	bool is_segment_enabled = r_segmentation.IsSegmentEnabled(mSegmentId);

	//if segment is already selected
	if (is_segment_enabled) {
		string action_str = string("Disable Segment");
		addAction(QString(action_str.c_str()), this, SLOT(Disable()));

	} else {
		string action_str = string("Enable Segment");
		addAction(QString("Enable Segment"), this, SLOT(Enable()));
	}

	//addAction( QString("Disable Selected"), this, SLOT(DisableSelected()));

	addAction(QString("Disable Unselected"), this, SLOT(DisableUnselected()));

	addAction(QString("Enable All"), this, SLOT(EnableAll()));
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
	sel.selectJustThisSegment( mSegmentId, true);
	sel.sendEvent();
}

void OmSegmentContextMenu::Unselect()
{
	OmSegmentSelector sel(mSegmentationId, this, "view3d" );
	sel.selectJustThisSegment( mSegmentId, false);
	sel.sendEvent();
}

void OmSegmentContextMenu::UnselectOthers()
{
	OmSegmentSelector sel(mSegmentationId, this, "view3d" );
	sel.selectNoSegments();
	sel.selectJustThisSegment( mSegmentId, true);
	sel.sendEvent();
}

void OmSegmentContextMenu::Disable()
{
	printf("%s: should be removed...\n", __FUNCTION__);
	//(new OmSegmentStateAction(mSegmentationId, mSegmentId, false))->Run();
}

void OmSegmentContextMenu::Enable()
{
	printf("%s: should be removed...\n", __FUNCTION__);
	//(new OmSegmentStateAction(mSegmentationId, mSegmentId, true))->Run();
}

void OmSegmentContextMenu::DisableSelected()
{
	printf("%s: should be removed...\n", __FUNCTION__);
	//	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);
	//(new OmSegmentStateAction(mSegmentationId, r_segmentation.GetSelectedSegmentIds(), false))->Run();
}

void OmSegmentContextMenu::DisableUnselected()
{
	printf("%s: should be removed...\n", __FUNCTION__);
	
	//	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);

	//form unselected
	//	OmIDsSet unselected_segments = seg.GetEnabledSegmentIds() - seg.GetSelectedSegmentIds();

	//	(new OmSegmentStateAction(mSegmentationId, unselected_segments, false))->Run();
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
}

void OmSegmentContextMenu::addGroup()
{
        debug("validate", "OmSegmentContextMenu::addGroup\n");
        if (OmProject::IsSegmentationValid(mSegmentationId)) {
                OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
                seg.AddGroup(seg.GetSegment(seg.GetSegment(mSegmentId)->getRootSegID())->getValue());
        }
}

void OmSegmentContextMenu::deleteGroup()
{
        debug("validate", "OmSegmentContextMenu::addGroup\n");
        if (OmProject::IsSegmentationValid(mSegmentationId)) {
                OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
                seg.DeleteGroup(seg.GetSegment(seg.GetSegment(mSegmentId)->getRootSegID())->getValue());
        }
}

