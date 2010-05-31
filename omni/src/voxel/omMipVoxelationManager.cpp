
#include "omMipVoxelationManager.h"

#include "omMipVoxelation.h"
#include "voxel/omMipSegmentDataCoord.h"

#include "segment/omSegmentCache.h"
#include "volume/omDrawOptions.h"
#include "volume/omMipVolume.h"

#include "system/omStateManager.h"
#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"

#include "common/omDebug.h"
#include "common/omGl.h"

#define DEBUG 0

/////////////////////////////////
///////
///////          OmMipVoxelationManager
///////

OmMipVoxelationManager::OmMipVoxelationManager(OmMipVolume * pMipVolume)
 : MipVoxelationCache(VRAM_CACHE_GROUP, false), mpMipVolume(pMipVolume)
{
	debug("genone", "OmMipVoxelationManager::OmMipVoxelationManager\n");
	//set cache properties
	SetFetchUpdateInterval(0.5f);
	//flushes fetch stack so it doesn't waste time fetching old requests
	SetFetchUpdateClearsFetchStack(false);

	/** Set The Name of the Cache */
        SetCacheName("OmMipVoxelationManager");
}

OmMipVoxelationManager::~OmMipVoxelationManager()
{

}

/////////////////////////////////
///////          Property Accessors

/////////////////////////////////
///////          Voxelation Accessors

void
OmMipVoxelationManager::GetVoxelation(QExplicitlySharedDataPointer < OmMipVoxelation > &p_value, const OmMipSegmentDataCoord & coord)
{
	MipVoxelationCache::Get(p_value, coord, false);
}

bool OmMipVoxelationManager::ContainsVoxelation(const OmMipSegmentDataCoord & coord)
{
	return MipVoxelationCache::Contains(coord);
}

void OmMipVoxelationManager::RemoveVoxelation(const OmMipSegmentDataCoord & coord)
{
	MipVoxelationCache::Remove(coord);
}

/////////////////////////////////
///////          Voxelation Generation

/*
 *	Determine if the voxel at the given data coordinate is a boundary voxel.
 *	A boundary voxel is defined to be a non-null voxel with a neighbor
 *	that has a value other than itself.
 *
 *	note: voxels on edges of volume are also consider boundary voxels
 */
bool OmMipVoxelationManager::IsBoundaryVoxel(const DataCoord & rDataCoord)
{

	//get voxel segment id
	const OmSegID voxel_value = mpMipVolume->GetVoxelValue(rDataCoord);

	//null ids are not boundaries
	if (NULL_SEGMENT_DATA == voxel_value)
		return false;

	//check all neightbors
	DataCoord neighbor_coord;

	//for all neighboring voxels
	for (int z = -1; z <= 1; ++z) {
		for (int y = -1; y <= 1; ++y) {
			for (int x = -1; x <= 1; ++x) {

				//skip original voxel position
				if ((x == 0) && (y == 0) && (z == 0)) {
					continue;
				}

				//form neighbor coord
				neighbor_coord = rDataCoord + Vector3 < int >(x, y, z);

				//invalid coordinates indicates boundary
				if (!mpMipVolume->ContainsVoxel(neighbor_coord)) {
					return true;
				}

				//if neighbor segment different from source
				if (voxel_value != mpMipVolume->GetVoxelValue(neighbor_coord)) {
					return true;
				}
			}
		}
	}

	//otherwise all neighbors were the same as vox_seg_id
	return false;
}

/*
 *	Create a MipVoxelation given a MipSegmentDataCoord to specify
 *	the MipChunk and value being voxelated.
 */

OmMipVoxelation *OmMipVoxelationManager::CreateMipVoxelation(const OmMipSegmentDataCoord & coord)
{

	debug("genone","OmMipVoxelationManager::CreateMipVoxelation\n");

	//alloc a new voxelation with this cache
	OmMipVoxelation *p_voxelation = new OmMipVoxelation(coord, this);

	//only leaf chunks should be voxelized
	assert(coord.MipChunkCoord.Level == 0);

	//get data extent
	DataBbox data_bbox = mpMipVolume->MipCoordToDataBbox(coord.MipChunkCoord, 0);
	Vector3 < int >extent_min = data_bbox.getMin();
	Vector3 < int >extent_max = data_bbox.getMax();

	//for all voxels in the chunk
	for (int z = extent_min.z; z <= extent_max.z; ++z)
		for (int y = extent_min.y; y <= extent_max.y; ++y)
			for (int x = extent_min.x; x <= extent_max.x; ++x) {

				//form data coordinate
				DataCoord vox = DataCoord(x, y, z);

				//ignore voxels of non-matching values
				if (mpMipVolume->GetVoxelValue(vox) != coord.DataValue)
					continue;

				//else part of segment, so check if boundary voxel              
				if (IsBoundaryVoxel(vox)) {
					//add boundary voxel to voxelation
					p_voxelation->AddVoxel(vox);
				}
			}

	//return voxelation with added voxels
	return p_voxelation;
}

/*
 *	Refresh the 3x3x3-voxel region centered at rDataCoord for the given MipVoxelation.
 *	The voxesl within the region are tested so that only only boundary voxels are in 
 *	the MipVoxelation.
 */
void OmMipVoxelationManager::RefreshNeighboringVoxels(const DataCoord & rVox, OmMipVoxelation & rVoxelation)
{

	//debug("FIXME", << "OmMipVoxelationManager::RefreshNeighboringVoxels:" << rVox << endl;

	DataCoord neighbor_coord;
	for (int z = -1; z <= 1; ++z)
		for (int y = -1; y <= 1; ++y)
			for (int x = -1; x <= 1; ++x) {
				//form neighbor coord
				neighbor_coord = rVox + DataCoord(x, y, z);

				//if invalid, skip
				if (!mpMipVolume->ContainsVoxel(neighbor_coord))
					continue;

				//remove voxel
				rVoxelation.RemoveVoxel(neighbor_coord);

				//if not part of segment, skip
				if (mpMipVolume->GetVoxelValue(neighbor_coord) != rVoxelation.GetSegmentDataValue())
					continue;

				//if boundary voxel
				if (IsBoundaryVoxel(neighbor_coord)) {
					//debug("FIXME", << "OmMipVoxelationManager::RefreshNeighboringVoxels: " << neighbor_coord << " is boundary" << endl;
					//add to voxelation
					rVoxelation.AddVoxel(neighbor_coord);
				}

			}
}

/*
 *	Change the value of a voxel.
 *
 *	1. if old value was voxelated (cache contains associated OmMipSegmentDataCoord)
 *		- refresh to remove voxel from OmMipVoxelation and add boundary neighbors
 *	2. if new value was voxelated (cache contains associated OmMipSegmentDataCoord)
 *		- refresh to add voxel to OmMipVoxelation and remove non-boundary neighbors
 */

void OmMipVoxelationManager::UpdateVoxel(const DataCoord & rVox,
					 const OmSegID old_val, const OmSegID new_val)
{

	//debug("FIXME", << "OmMipVoxelationManager::UpdateVoxel: " << rVox << " old: " << old_val << " new:" << new_val << endl;

	//determine which leaf mip chunk the voxel is in
	OmMipChunkCoord mip_chunk_coord = mpMipVolume->DataToMipCoord(rVox, 0);

	//form old mip data coord
	OmMipSegmentDataCoord old_mip_data_coord = OmMipSegmentDataCoord(mip_chunk_coord, old_val);

	//if old value  is voxelated
	if (ContainsVoxelation(old_mip_data_coord)) {
		//get voxelation
		QExplicitlySharedDataPointer < OmMipVoxelation > p_voxelation;
		GetVoxelation(p_voxelation, old_mip_data_coord);
		//refresh to add neighbor boundary voxels
		RefreshNeighboringVoxels(rVox, *p_voxelation);
	}
	//form new mip data coord
	OmMipSegmentDataCoord new_mip_data_coord = OmMipSegmentDataCoord(mip_chunk_coord, new_val);

	//if old value  is voxelated
	if (ContainsVoxelation(new_mip_data_coord)) {
		//get voxelation
		QExplicitlySharedDataPointer < OmMipVoxelation > p_voxelation;
		GetVoxelation(p_voxelation, new_mip_data_coord);
		//refresh to remove non-boundary neighbors
		RefreshNeighboringVoxels(rVox, *p_voxelation);
	}

}

/////////////////////////////////
///////          Cache Handles

/*
 *	Cache miss causes a fetch from disk for mesh that corresponds
 *	to the given MeshCoord.
 */
OmMipVoxelation *OmMipVoxelationManager::HandleCacheMiss(const OmMipSegmentDataCoord & coord)
{
	//debug("genone","OmMipVoxelationManager::HandleCacheMiss: ");
	return CreateMipVoxelation(coord);
}

void OmMipVoxelationManager::HandleFetchUpdate()
{
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
	//debug("genone","OmMipVoxelationManager::FetchUpdate: redraw sent");
}

/////////////////////////////////
///////          Draw

void OmMipVoxelationManager::DrawVoxelations(OmSegmentCache * rSegMgr,
					     const OmMipChunkCoord & mipCoord,
					     const OmSegIDs & rRelvDataVals, 
					     const OmBitfield & /*drawOps*/)
{
	//push modelview matrix
	glPushMatrix();

	//transform to mipvolume voxel space
	Vector3f scale = Vector3f::ONE / mpMipVolume->GetExtent().getUnitDimensions();
	glScalefv(scale.array);

	//for all relevent data values in chunk
	foreach( OmSegID val, rRelvDataVals ){

		//get pointer to mesh
		QExplicitlySharedDataPointer < OmMipVoxelation > p_voxelation;
		GetVoxelation(p_voxelation, OmMipSegmentDataCoord(mipCoord, val));

		//if null pointer, then not in cache so skip to next mesh
		if (NULL == p_voxelation)
			continue;

		//determine which segment this data values belongs to
		OmSegment * r_segment = rSegMgr->GetSegmentFromValue(val);
		//r_segment->ApplyColor(drawOps, NULL);
		printf("FIXME: voxelation manager has been gutte\n");

		//draw mesh
		glPushName(r_segment->getValue());
		glPushName(OMGL_NAME_VOXEL);

		p_voxelation->Draw();

		glPopName();
		glPopName();

	}

	//return to normalized space
	glPopMatrix();
}
