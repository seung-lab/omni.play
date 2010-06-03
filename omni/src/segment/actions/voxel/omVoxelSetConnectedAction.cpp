#include "project/omProject.h"
#include "omVoxelSetConnectedAction.h"

#include "segment/actions/omSegmentEditor.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#include "system/omStateManager.h"

/////////////////////////////////
///////
///////          OmVoxelSetAction Class
///////

OmVoxelSetConnectedAction::OmVoxelSetConnectedAction()
{

	//store current selection
	bool valid_edit_selection = OmSegmentEditor::GetEditSelection(mSegmentationId, mSegmentId);

	//if edit selection not valid
	if (!valid_edit_selection) {
		cout << "OmVoxelSetConnectedAction: edit selection not valid" << endl;
		OmAction::SetValid(false);
		return;
	}

	//get selected voxels
	const set < DataCoord > &r_selected_voxels = OmSegmentEditor::GetSelectedVoxels();

	//if voxel selection not valid
	if (1 != r_selected_voxels.size()) {
		cout << "OmVoxelSetConnectedAction: select only one voxel" << endl;
		OmAction::SetValid(false);
		return;
	}

	//else get voxel
	mSeedVoxel = *r_selected_voxels.begin();

	//get segmentation
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

	//valid edit selection, so get dest data value
	mDataValue = mSegmentId;

	//get current voxel id
	mSeedSegmentId = r_segmentation.GetVoxelSegmentId(mSeedVoxel);

	//if voxel selection not valid
	if( 0 == mSeedSegmentId) {
		cout << "OmVoxelSetConnectedAction: cannot set connected null ids" << endl;
		OmAction::SetValid(false);
		return;
	}

	//non-undoable action
	SetUndoable(false);
}

/////////////////////////////////
///////          Action Methods

void OmVoxelSetConnectedAction::AddConnectedNeighborsToList(OmSegmentation & rSegmentation, 
							    const DataCoord & srcVox,
							    list < DataCoord > &todoList)
{
	for (int z = -1; z <= 1; ++z) {
		for (int y = -1; y <= 1; ++y) {
			for (int x = -1; x <= 1; ++x) {

				//fast center check
				//if(0 == (x | y | z)) continue;

				//get offset coordinate
				DataCoord offset_vox = srcVox + DataCoord(x, y, z);

				//skip if invalid voxel
				if (!rSegmentation.ContainsVoxel(offset_vox))
					continue;

				//if voxel seg id matches, then add to list
				if (rSegmentation.GetVoxelSegmentId(offset_vox) == mSeedSegmentId) {
					//cout << endl;
					//cout << srcSegId << endl;
					//cout << rSegmentation.GetVoxelSegmentId(offset_vox) << endl;

					//set voxel to new data value
					rSegmentation.SetVoxelValue(offset_vox, mDataValue);
					todoList.push_back(offset_vox);
				}

			}
		}
	}
}

void OmVoxelSetConnectedAction::Action()
{

	//get segmentation
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

	//alloc todo set on heap in case very large
	list < DataCoord > *vox_todo_list = new list < DataCoord >;
	//add seed voxel
	vox_todo_list->push_back(mSeedVoxel);

	//while voxels todo
	DataCoord cur_vox;
	while (vox_todo_list->size()) {

		cout << vox_todo_list->size() << endl;

		//get front
		cur_vox = vox_todo_list->front();

		//pop front
		vox_todo_list->pop_front();

		//push connected neighbors
		AddConnectedNeighborsToList(r_segmentation, cur_vox, *vox_todo_list);

	}

	//delete list
	delete vox_todo_list;

}

void OmVoxelSetConnectedAction::UndoAction()
{
	assert(false);
}

string OmVoxelSetConnectedAction::Description()
{
	return "Set Connected Voxels";
}
