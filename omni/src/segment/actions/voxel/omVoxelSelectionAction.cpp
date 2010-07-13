#include "datalayer/fs/omActionLoggerFS.h"
#include "omVoxelSelectionAction.h"
#include "segment/actions/omSegmentEditor.h"

/////////////////////////////////
///////
///////          OmVoxelSelectionAction Class
///////

OmVoxelSelectionAction::OmVoxelSelectionAction(const DataCoord & voxel, bool state, bool append)
	: mVoxel(voxel)
	, mNewState(state)
	, mAppend(append)
{

	//store prev voxel state
	mOldState = OmSegmentEditor::GetSelectedVoxelState(voxel);

	//if not appending then store prev selected
	if (!mAppend) {
		mPrevSelectedVoxels = OmSegmentEditor::GetSelectedVoxels();
	}

}

/////////////////////////////////
///////          Action Methods

void OmVoxelSelectionAction::Action()
{

	//if not appending, then clear other voxels
	if (!mAppend)
		OmSegmentEditor::SetSelectedVoxels(set < DataCoord > ());

	//set new selected state of all voxels
	OmSegmentEditor::SetSelectedVoxelState(mVoxel, mNewState);
}

void OmVoxelSelectionAction::UndoAction()
{

	//restore selected state of all voxels
	OmSegmentEditor::SetSelectedVoxelState(mVoxel, mOldState);

	//if not append, restore prev selected voxels
	if (!mAppend)
		OmSegmentEditor::SetSelectedVoxels(mPrevSelectedVoxels);
}

string OmVoxelSelectionAction::Description()
{
	if (mNewState)
		return "Voxel Selected";
	else
		return "Voxel Unselected";
}

void OmVoxelSelectionAction::save(const string & comment)
{
	OmActionLoggerFS().save(this, comment);
}
