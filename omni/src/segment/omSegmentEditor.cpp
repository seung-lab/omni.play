
#include "omSegmentEditor.h"

#include "volume/omVolume.h"
#include "volume/omSegmentation.h"

#include "system/omEventManager.h"
#include "system/events/omSegmentEvent.h"
#include "system/omDebug.h"

#define DEBUG 0



//init instance pointer
OmSegmentEditor* OmSegmentEditor::mspInstance = 0;




#pragma mark -
#pragma mark OmStateManager
/////////////////////////////////
///////
///////		 OmStateManager
///////

OmSegmentEditor::OmSegmentEditor() { 
	mEditSegmentation = NULL_OM_ID;
	mEditSegment = NULL_OM_ID;
}

OmSegmentEditor::~OmSegmentEditor() { 
	
}




OmSegmentEditor* 
OmSegmentEditor::Instance() {
	if(NULL == mspInstance) {
		mspInstance = new OmSegmentEditor;
	}
	return mspInstance;
}


void 
OmSegmentEditor::Delete() {
	if(mspInstance) delete mspInstance;
	mspInstance = NULL;
}








#pragma mark 
#pragma mark Edit Selection
/////////////////////////////////
///////		 Edit Selection

void 
OmSegmentEditor::SetEditSelection(OmId segmentation, OmId segment) {
	Instance()->mEditSegmentation = segmentation;
	Instance()->mEditSegment = segment;
	
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_EDIT_SELECTION_CHANGE));
}


bool 
OmSegmentEditor::GetEditSelection(OmId &segmentation, OmId &segment) {
	segmentation = Instance()->mEditSegmentation;
	segment = Instance()->mEditSegment;
	
	//check for segmentation validity
	if(!OmVolume::IsSegmentationValid(segmentation))
		return false;
	
	//check for segment validity
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(segmentation);
	if(!r_segmentation.IsSegmentValid(segment))
		return false;
	
	//both valid
	return true;
}







#pragma mark 
#pragma mark Selected Voxels
/////////////////////////////////
///////		 Selected Voxels

const set< DataCoord >&
OmSegmentEditor::GetSelectedVoxels() {
	return Instance()->mSelectedVoxels;
}

void 
OmSegmentEditor::SetSelectedVoxels(const set< DataCoord > &voxels) {
	Instance()->mSelectedVoxels = voxels;
}

void
OmSegmentEditor::ClearSelectedVoxels() {
	Instance()->mSelectedVoxels.clear();
}


void 
OmSegmentEditor::SetSelectedVoxelState(const DataCoord &dataCoord, bool selected) {
	if(selected)
		Instance()->mSelectedVoxels.insert(dataCoord);
	else
		Instance()->mSelectedVoxels.erase(dataCoord);
}

bool 
OmSegmentEditor::GetSelectedVoxelState(const DataCoord &dataCoord) {
	return Instance()->mSelectedVoxels.count(dataCoord);
}








#pragma mark 
#pragma mark Voxels
/////////////////////////////////
///////		 Voxels

DataCoord 
OmSegmentEditor::SpaceCoordToEditSelectionVoxel(const SpaceCoord &spaceCoord) {
	
	//convert to norm coord
	NormCoord normCoord = OmVolume::SpaceToNormCoord(spaceCoord);
	//convert to data coord
	DataCoord dataCoord = OmVolume::NormToDataCoord(normCoord);
	
	return dataCoord;
}





#pragma mark 
#pragma mark Draw
/////////////////////////////////
///////		 Draw

void 
OmSegmentEditor::DrawEditSelectionVoxels() {
	//push modelview matrix
	glPushMatrix();
	
	Vector3f scale = Vector3f::ONE / OmVolume::GetDataExtent().getUnitDimensions();
	glScalefv(scale.array);
	
	//all omglSolidCubes are centered at 0,0,0 so offset by 1/2
	glTranslatef(0.5,0.5,0.5);
	glColor3f(1.0f, 1.0f, 0.0f);
	
	Vector3f prev_voxel_coord = Vector3f::ZERO;
	Vector3f cur_voxel_coord; 
	Vector3f offset;	//from prev to current
	
	//copy edit selection voxels in case set is edited
	set< DataCoord > edit_selection_voxels = GetSelectedVoxels();
	
	//for all voxels in set
	set< DataCoord >::iterator vox_itr;
	for( vox_itr=edit_selection_voxels.begin(); vox_itr != edit_selection_voxels.end(); vox_itr++ ) {
		
		//get current voxel coordinate
		cur_voxel_coord = *vox_itr;
		//calculate needed offset
		offset = cur_voxel_coord - prev_voxel_coord;
		//store current as prev
		prev_voxel_coord = cur_voxel_coord; 
		
		//translate offset
		glTranslatefv(offset.array);
		
		//draw cube
		omglSolidCube(1);
	}
	
	
	glPopMatrix();
}


