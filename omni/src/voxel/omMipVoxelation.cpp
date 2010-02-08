
#include "omMipVoxelation.h"
#include "omMipVoxelationManager.h"

#include "voxel/omMipSegmentDataCoord.h"
#include "common/omGl.h"
#include "system/omDebug.h"

#define DEBUG 0

#pragma mark -
#pragma mark Example Class
/////////////////////////////////
///////
///////          Example Class
///////

OmMipVoxelation::OmMipVoxelation(const OmMipSegmentDataCoord & mipSegDataCoord, OmMipVoxelationManager * pCache)
:OmCacheableBase(pCache)
{

	//init coordinate
	mCoordinate = mipSegDataCoord;
}

OmMipVoxelation::~OmMipVoxelation()
{

}

#pragma mark
#pragma mark Accessor Methods
/////////////////////////////////
///////          Accessor Methods

SEGMENT_DATA_TYPE OmMipVoxelation::GetSegmentDataValue()
{
	return mCoordinate.DataValue;
}

#pragma mark
#pragma mark Example Methods
/////////////////////////////////
///////          Example Methods

void OmMipVoxelation::AddVoxel(DataCoord & vox)
{
	mVoxels.insert(vox);
}

void OmMipVoxelation::RemoveVoxel(DataCoord & vox)
{
	mVoxels.erase(vox);
}

#pragma mark
#pragma mark Draw Methods
/////////////////////////////////
///////          Draw Methods

void OmMipVoxelation::Draw()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	//all omglSolidCubes are centered at 0,0,0 so offset by 1/2
	glTranslatef(0.5, 0.5, 0.5);

	Vector3f prev_voxel_coord = Vector3f::ZERO;
	Vector3f cur_voxel_coord;
	Vector3f offset;	//from prev to current

	//for all voxels in set
	set < DataCoord >::iterator vox_itr;
	for (vox_itr = mVoxels.begin(); vox_itr != mVoxels.end(); vox_itr++) {

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

		//draw wire outline
		//glColor3f(0.0f, 0.0f, 0.0f);
		//glLineWidth(1);
		//omglWireCube(1);
	}

	glPopMatrix();
	glPopAttrib();

}
