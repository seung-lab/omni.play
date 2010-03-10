
#include "omEditSelectionApplyAction.h"

#include "segment/omSegmentEditor.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#define DEBUG 0

/////////////////////////////////
///////
///////          OmEditSelectionApplyAction Class
///////

OmEditSelectionApplyAction::OmEditSelectionApplyAction()
{
	//store current volume and segmentation
	OmSegmentEditor::GetEditSelection(mSegmentationId, mSegmentId);

	//get segmentation
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//map current voxel selection to current ids (for undo)
	const set < DataCoord > &r_selected_voxels = OmSegmentEditor::GetSelectedVoxels();

	//for all selected voxels, map voxel to id
	set < DataCoord >::iterator itr;
	for (itr = r_selected_voxels.begin(); itr != r_selected_voxels.end(); itr++) {
		//get segment id value
		OmId segment_id = r_segmentation.GetVoxelSegmentId(*itr);
		//map voxel
		mVoxelMap[*itr] = segment_id;
	}

}

/////////////////////////////////
///////          Action Methods

void
 OmEditSelectionApplyAction::Action()
{

	//get segmentation
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//for all voxels in the map, set value to mSegmentId
	map < DataCoord, OmId >::iterator itr;
	for (itr = mVoxelMap.begin(); itr != mVoxelMap.end(); itr++) {

		//get ref to voxel
		const DataCoord & r_voxel = (*itr).first;

		//set voxel to stored segment id
		r_segmentation.SetVoxelSegmentId(r_voxel, mSegmentId);

		//voxel is unselected
		OmSegmentEditor::SetSelectedVoxelState(r_voxel, false);
	}
}

void OmEditSelectionApplyAction::UndoAction()
{
	//to undo, apply mapped value to each voxel key in the map

	//get segmentation
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//for all voxels in the map, set value to mSegmentId
	map < DataCoord, OmId >::iterator itr;
	for (itr = mVoxelMap.begin(); itr != mVoxelMap.end(); itr++) {

		//get ref to voxel
		const DataCoord & r_voxel = (*itr).first;

		//set voxel to stored segment id
		r_segmentation.SetVoxelSegmentId(r_voxel, mVoxelMap[r_voxel]);

		//voxel is reselected
		OmSegmentEditor::SetSelectedVoxelState(r_voxel, true);
	}

}

string OmEditSelectionApplyAction::Description()
{
	return "Apply Edit Selection";
}
