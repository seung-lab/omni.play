
#include "omSegmentation.h"
#include "omVolume.h"
#include "omMipChunk.h"
#include "omVolumeCuller.h"
#include "mesh/meshingManager.h"

#include "segment/omSegmentEditor.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "system/omEventManager.h"
#include "project/omProject.h"
#include "system/events/omProgressEvent.h"
#include "system/events/omView3dEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/omSystemTypes.h"
#include "common/omUtility.h"

#include <vtkImageData.h>

#include <vmmlib/vmmlib.h>
#include "common/omDebug.h"
#include <QFile>
#include <QTextStream>
using namespace vmml;

#define DEBUG 0

#pragma mark -
#pragma mark OmSegmentation
/////////////////////////////////
///////
///////         OmSegmentation
///////

OmSegmentation::OmSegmentation()
 : mMipVoxelationManager(this)
{
	SetBytesPerSample(SEGMENT_DATA_BYTES_PER_SAMPLE);

	/** Set The Name of the Cache */
        SetCacheName("OmMipMeshManager");
}

OmSegmentation::OmSegmentation(OmId id)
 : OmManageableObject(id), mMipVoxelationManager(this)
{

	//set manageable object name
	char idchar[25];
	snprintf(idchar, sizeof(idchar), "%i", id);
	SetName("segmentation" + string(idchar));

	//set permenant directory name
	SetDirectoryPath(string("segmentations/") + GetName() + string("/"));

	//segmenations has SEGMENT_DATA_BYTES_PER_SAMPLE bytes per sample
	SetBytesPerSample(SEGMENT_DATA_BYTES_PER_SAMPLE);
	//debug("FIXME", << "Setting BPP to " << SEGMENT_DATA_BYTES_PER_SAMPLE << endl;

	//subsample segmentation data using mode
	SetSubsampleMode(SUBSAMPLE_NONE);

	//uses meta data
	mStoreChunkMetaData = true;

	//build blank data
	BuildVolumeData();
}

#pragma mark
#pragma mark Property Accessors
/////////////////////////////////
///////          Property Accessors

void OmSegmentation::SetDirectoryPath(string dpath)
{
	//call parent
	OmMipVolume::SetDirectoryPath(dpath);
	//set associated
	mSegmentManager.SetDirectoryPath(dpath);
	mMipMeshManager.SetDirectoryPath(dpath);
}

#pragma mark
#pragma mark Data Mapping
/////////////////////////////////
///////          Data Mapping

/*
 *	Get the Segment OmId mapped to the given data value.
 */
OmId OmSegmentation::GetSegmentIdMappedToValue(SEGMENT_DATA_TYPE value)
{
	return mSegmentManager.GetSegmentIdMappedToValue(value);
}

/*
 *	Get the set of data values that map to the given Segment OmId.
 */
const SegmentDataSet & OmSegmentation::GetValuesMappedToSegmentId(OmId omId)
{
	return mSegmentManager.GetValuesMappedToSegmentId(omId);
}

/*
 *	Returns one of the values mapped to a segment id.
 */
SEGMENT_DATA_TYPE OmSegmentation::GetValueMappedToSegmentId(OmId omId)
{
	//debug("FIXME", << "OmSegmentation::GetValueMappedToSegmentId: " << omId << endl;

	if (IsSegmentValid(omId)) {
		//if valid return first in set
		const SegmentDataSet & r_data_set = GetValuesMappedToSegmentId(omId);
		assert(r_data_set.size());
		return *(r_data_set.begin());
	} else {
		//debug("FIXME", << "OmSegmentation::GetValueMappedToSegmentId: invalid segment id: " << omId << endl;
		assert(false);
	}

}

void OmSegmentation::MapValuesToSegmentId(OmId omId, const SegmentDataSet & values)
{
	mSegmentManager.MapValuesToSegmentId(omId, values);
}

void OmSegmentation::UnMapValuesToSegmentId(OmId omId, const SegmentDataSet & values)
{
	mSegmentManager.UnMapValuesToSegmentId(omId, values);
}

#pragma mark
#pragma mark Data Accessors
/////////////////////////////////
///////          Data Accessors

/*
 *	Overridden so as to update MipVoxelationManager
 */
void OmSegmentation::SetVoxelValue(const DataCoord & rVox, uint32_t val)
{

	//debug("FIXME", << "OmSegmentation::SetVoxelValue: " << rVox << " , " << val << endl;

	//get old value
	uint32_t old_val = GetVoxelValue(rVox);

	//update data
	OmMipVolume::SetVoxelValue(rVox, val);

	//change voxel in voxelation
	mMipVoxelationManager.UpdateVoxel(rVox, old_val, val);
}

/*
 *	Use the first data value of the voxel to determine which
 *	SegmentId the voxel is mapped to.
 */
OmId OmSegmentation::GetVoxelSegmentId(const DataCoord & vox)
{
	SEGMENT_DATA_TYPE data_value = GetVoxelValue(vox);
	return GetSegmentIdMappedToValue(data_value);
}

/*
 *	Convienence method that sets the data value of the voxel to
 *	any value that is mapped to the given SegmentId
 */
void OmSegmentation::SetVoxelSegmentId(const DataCoord & vox, OmId omId)
{
	//set voxel to first value in set
	SetVoxelValue(vox, GetValueMappedToSegmentId(omId));
}

#pragma mark
#pragma mark Build Methods
/////////////////////////////////
///////          Build Methods

bool OmSegmentation::IsVolumeDataBuilt()
{
	return OmMipVolume::IsBuilt();
}

void OmSegmentation::BuildVolumeData()
{

	//init progress bar
	//string progress_bar_str = string("Building Segmentation Volume: ") + GetName();
	//OmEventManager::PostEvent(new OmProgressEvent(OmProgressEvent::PROGRESS_SHOW, progress_bar_str));

	//build volume
	OmMipVolume::Build();

	//hide progress bar
	//OmEventManager::PostEvent( new OmProgressEvent(OmProgressEvent::PROGRESS_HIDE));

	//seg change event
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));

	// disable all segments by default
	SetAllSegmentsSelected(false);
	SetAllSegmentsEnabled(false);

	OmProject::Save();
	printf("done building segmentation data\n");
}

/*
 *	Returns true if BuildMeshData() has previously been called successfully.
 */
bool OmSegmentation::IsMeshDataBuilt()
{
	return mMipMeshManager.IsMeshDataBuilt();
}

/*
 *	Build all meshes in all chunks of the MipVolume.
 */
void OmSegmentation::BuildMeshData()
{

	if (!IsVolumeDataBuilt())
		throw OmAccessException(string("Segmentation volume data must be built before mesh data: ") +
					GetName());

	//init progress bar
	int num_chunks_in_build = MipChunksInVolume();
	//string progress_bar_str = string("Building Segmentation Meshes: ") + GetName();
	//OmEventManager::PostEvent(new OmProgressEvent(OmProgressEvent::PROGRESS_SHOW, progress_bar_str, 0, num_chunks_in_build));

	//build all levels
	//BuildMeshDataInternal();
	BuildMeshDataInternalNew();

	//all chunks have been meshed
	mMipMeshManager.SetMeshDataBuilt(true);

	//hide progress bar
	//OmEventManager::PostEvent(new OmProgressEvent(OmProgressEvent::PROGRESS_HIDE));

	OmProject::Save();
}

/*
 * Produce the plan file.
 */
void OmSegmentation::BuildMeshDataPlan(const QString & planFile)
{
        if (!IsVolumeDataBuilt())
                throw OmAccessException(string("Segmentation volume data must be built before mesh data: ") +
                                        GetName());

    	QFile file(planFile);
    	//QFile file("file.txt");
    	if ( !file.open(QIODevice::WriteOnly)) {
		throw (false && "couldn't open the plan file, check to make sure you have write permission");
	}
        QTextStream stream(&file);

        //for each level
        for (int level = 0; level <= GetRootMipLevel(); ++level) {

                //dim of leaf coords
                Vector3 < int >mip_coord_dims = MipLevelDimensionsInMipChunks(level);

                //for all coords of level
                for (int z = 0; z < mip_coord_dims.z; ++z) {
                        for (int y = 0; y < mip_coord_dims.y; ++y) {
                                for (int x = 0; x < mip_coord_dims.x; ++x) {
					stream << "meshchunk:" << GetId() << ":" << level << ":" << x << "," << y << "," << z << endl;
                                }
			}
		}
        }
        file.close();
}


/*
 * Build the meshes for a single chunk.
 */
void OmSegmentation::BuildMeshChunk(int level, int x, int y, int z)
{
	OmMipChunkCoord chunk_coord(level, x, y, z);
	shared_ptr < OmMipChunk > p_chunk = shared_ptr < OmMipChunk > ();
	GetChunk(p_chunk, chunk_coord);

	const SegmentDataSet & rMeshVals = p_chunk->GetDirectDataValues();
        if( 0 != rMeshVals.size() ){
        	mMipMeshManager.BuildChunkMeshes(p_chunk, rMeshVals );
        }
}


void OmSegmentation::BuildMeshDataInternal()
{

	//for each level
	for (int level = 0; level <= GetRootMipLevel(); ++level) {

		//dim of leaf coords
		Vector3 < int >mip_coord_dims = MipLevelDimensionsInMipChunks(level);

		//for all coords of level
		for (int z = 0; z < mip_coord_dims.z; ++z)
			for (int y = 0; y < mip_coord_dims.y; ++y)
				for (int x = 0; x < mip_coord_dims.x; ++x) {

					//form mip chunk coord
					OmMipChunkCoord chunk_coord(level, x, y, z);

					//get built chunk (hold shared pointer whild building)
					shared_ptr < OmMipChunk > p_chunk = shared_ptr < OmMipChunk > ();
					GetChunk(p_chunk, chunk_coord);

					const SegmentDataSet & rMeshVals = p_chunk->GetDirectDataValues();
					if( 0 != rMeshVals.size() ){
						mMipMeshManager.BuildChunkMeshes(p_chunk, rMeshVals );
					}

					//update progress
					//OmEventManager::PostEvent(new OmProgressEvent(OmProgressEvent::PROGRESS_INCREMENT));
				}
	}
}


void OmSegmentation::BuildMeshDataInternalNew()
{
	MeshingManager* meshingMan = new MeshingManager( GetId(), &mMipMeshManager );

	for (int level = 0; level <= GetRootMipLevel(); ++level) {

		Vector3 < int >mip_coord_dims = MipLevelDimensionsInMipChunks(level);

		for (int z = 0; z < mip_coord_dims.z; ++z) {
			for (int y = 0; y < mip_coord_dims.y; ++y) {
				for (int x = 0; x < mip_coord_dims.x; ++x) {
					OmMipChunkCoord chunk_coord(level, x, y, z);
					meshingMan->addToQueue( chunk_coord );
				}
			}
		}
	}
	
	meshingMan->start();
	meshingMan->wait();
}

/*
 *	Overridden BuildChunk method so that the mesh data for a chunk will
 *	also be rebuilt if needed.
 */
void OmSegmentation::BuildChunk(const OmMipChunkCoord & mipCoord)
{
	//debug("FIXME", << "OmSegmentation::BuildChunk(): " << mipCoord << endl;

	//build chunk volume data
	OmMipVolume::BuildChunk(mipCoord);

	//get pointer to chunk
	shared_ptr < OmMipChunk > p_chunk = shared_ptr < OmMipChunk > ();
	GetChunk(p_chunk, mipCoord);

	//analyze entire chunk segmentation data
	p_chunk->RefreshDirectDataValues();
	p_chunk->RefreshIndirectDataValues();

	//if root then update segment manager with spacial content information
	if (p_chunk->IsRoot()) {
		//get root spatial segs
		const SegmentDataSet & r_root_indirect_data_values = p_chunk->GetIndirectDataValues();
		//debug("FIXME", << "ROOT SEGMENTS:" << r_root_indirect_data_values.size() << endl;

		//add to manager if data isn't already mapped
		mSegmentManager.AddSegments(r_root_indirect_data_values);
	}

	//rebuild mesh data only if entire volume data has been built as well
	if (IsVolumeDataBuilt() && IsMeshDataBuilt()) {
		//get pointer to chunk
		shared_ptr < OmMipChunk > p_chunk = shared_ptr < OmMipChunk > ();
		GetChunk(p_chunk, mipCoord);

		//get modified values
		const SegmentDataSet & rModifiedValues = p_chunk->GetModifiedVoxelValues();

		//return if no values modified
		if (rModifiedValues.size() == 0) {
			//debug("FIXME", << "OmSegmentation::BuildChunk: no modified values to mesh" << endl;
			return;
		}
		//build mesh
		mMipMeshManager.BuildChunkMeshes(p_chunk, rModifiedValues);

		//remove mesh from cache to force it to reload
		SegmentDataSet::iterator itr;
		for (itr = rModifiedValues.begin(); itr != rModifiedValues.end(); itr++) {
			OmMipMeshCoord mip_mesh_coord = OmMipMeshCoord(mipCoord, *itr);
			mMipMeshManager.UncacheMesh(mip_mesh_coord);
		}
	}

}

void OmSegmentation::RebuildChunk(const OmMipChunkCoord & mipCoord, const SegmentDataSet & rModifiedValues)
{

	//build chunk volume data and analyze data
	OmMipVolume::BuildChunk(mipCoord);

	//rebuild mesh data only if entire volume data has been built
	if (IsVolumeDataBuilt()) {
		//get pointer to chunk
		shared_ptr < OmMipChunk > p_chunk = shared_ptr < OmMipChunk > ();
		GetChunk(p_chunk, mipCoord);
		//build mesh
		mMipMeshManager.BuildChunkMeshes(p_chunk, rModifiedValues);
	}
	//remove mesh from cache to force it to reload
	SegmentDataSet::iterator itr;
	for (itr = rModifiedValues.begin(); itr != rModifiedValues.end(); itr++) {
		OmMipMeshCoord mip_mesh_coord = OmMipMeshCoord(mipCoord, *itr);
		mMipMeshManager.UncacheMesh(mip_mesh_coord);
	}

	//call redraw to force mesh to reload
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}

#pragma mark
#pragma mark Export
/////////////////////////////////
///////          Export

/*
 *	Replace each value in the volume with its associated OmId.
 */
void OmSegmentation::ExportDataFilter(vtkImageData * pImageData)
{

	//get data extent (varify it is a chunk)
	int extent[6];
	pImageData->GetExtent(extent);

	//get pointer to native scalar data
	assert(pImageData->GetScalarSize() == SEGMENT_DATA_BYTES_PER_SAMPLE);
	SEGMENT_DATA_TYPE *p_scalar_data = static_cast < SEGMENT_DATA_TYPE * >(pImageData->GetScalarPointer());

	//for all voxels in the chunk
	int x, y, z;
	for (z = extent[0]; z <= extent[1]; z++)
		for (y = extent[2]; y <= extent[3]; y++)
			for (x = extent[4]; x <= extent[5]; x++) {

				//if non-null segment value
				if (NULL_SEGMENT_DATA != *p_scalar_data) {
					//then look up om id associated to value
					OmId om_id = GetSegmentIdMappedToValue(*p_scalar_data);
					//and replace in image data
					*p_scalar_data = om_id;
				}
				//adv to next scalar
				++p_scalar_data;
			}

}

#pragma mark
#pragma mark Event Handling
/////////////////////////////////
///////          Event Handling

void OmSegmentation::SystemModeChangeEvent(OmSystemModeEvent * event)
{

	//if change out of editing mode
	if (OmStateManager::GetSystemMode() != EDIT_SYSTEM_MODE) {
		//then build edited chunks
		OmMipVolume::Flush();
	}
}

#pragma mark
#pragma mark Segment Management
/////////////////////////////////
///////          Segment Management

OmSegment & OmSegmentation::GetSegment(OmId id)
{
	return mSegmentManager.GetSegment(id);
}

OmSegment & OmSegmentation::AddSegment()
{
	OmSegment& segment = mSegmentManager.AddSegment();
	OmProject::Save();
	return segment;
}

void OmSegmentation::RemoveSegment(OmId id)
{
	mSegmentManager.RemoveSegment(id);
}

bool OmSegmentation::IsSegmentValid(OmId omId)
{
	return mSegmentManager.IsSegmentValid(omId);
}

const set < OmId > & OmSegmentation::GetValidSegmentIds()
{
	return mSegmentManager.GetValidSegmentIds();
}

bool OmSegmentation::IsSegmentEnabled(OmId id)
{
	return mSegmentManager.IsSegmentEnabled(id);
}

void OmSegmentation::SetSegmentEnabled(OmId id, bool enable)
{
	mSegmentManager.SetSegmentEnabled(id, enable);
}

const set < OmId > & OmSegmentation::GetEnabledSegmentIds()
{
	return mSegmentManager.GetEnabledSegmentIds();
}

bool OmSegmentation::IsSegmentSelected(OmId id)
{
	return mSegmentManager.IsSegmentSelected(id);
}

void OmSegmentation::SetSegmentSelected(OmId id, bool selected)
{
	mSegmentManager.SetSegmentSelected(id, selected);
}

void OmSegmentation::SetAllSegmentsSelected(bool selected)
{
	mSegmentManager.SetAllSegmentsSelected(selected);
}

void OmSegmentation::SetAllSegmentsEnabled(bool enabled)
{
	mSegmentManager.SetAllSegmentsEnabled(enabled);
}

const OmIds & OmSegmentation::GetSelectedSegmentIds()
{
	return mSegmentManager.GetSelectedSegmentIds();
}

#pragma mark
#pragma mark Segment Data Values Management
/////////////////////////////////
///////          Segment Data Values Management

const SegmentDataSet & OmSegmentation::GetEnabledSegmentDataValues()
{
	return mSegmentManager.GetEnabledSegmentDataValues();
}

const SegmentDataSet & OmSegmentation::GetSelectedSegmentDataValues()
{
	return mSegmentManager.GetSelectedSegmentDataValues();
}

const SegmentDataSet & OmSegmentation::GetUnselectedSegmentDataValues()
{
	return mSegmentManager.GetUnselectedSegmentDataValues();
}

const SegmentDataSet & OmSegmentation::GetVoxelizedSegmentDataValues()
{
	static SegmentDataSet empty;
	return empty;
}

#pragma mark
#pragma mark Draw
/////////////////////////////////
///////          Draw

/*
 *	Draw the entire Segmentation.  Starts the recursive processes of drawing MipChunks
 *	from the root MipChunk of the Segmentation.  Filters for relevant data values to be 
 *	drawn depending on culler draw options and passes relevant set to root chunk.
 */
void OmSegmentation::Draw(const OmVolumeCuller & rCuller)
{

	//return if mesh data not yet built
	if (!IsMeshDataBuilt())
		return;

	//initial relevant data values set
	const SegmentDataSet *p_relv_data_vals;

	//check to filter for relevant data values
	if (rCuller.CheckDrawOption(DRAWOP_SEGMENT_FILTER_SELECTED)) {
		//selected segment data values
		p_relv_data_vals = &mSegmentManager.GetSelectedSegmentDataValues();

	} else if (rCuller.CheckDrawOption(DRAWOP_SEGMENT_FILTER_UNSELECTED)) {
		//unselected segment data values
		p_relv_data_vals = &mSegmentManager.GetUnselectedSegmentDataValues();

	} else {
		//else no filtering, so all enabled segment data values
		p_relv_data_vals = &mSegmentManager.GetEnabledSegmentDataValues();
	}

	//return if empty data value set
	if (0 == p_relv_data_vals->size())
		return;

	//push segmentation name
	glPushName(GetId());

	//draw relevant data values starting from root chunk
	DrawChunkRecursive(RootMipChunkCoordinate(), *p_relv_data_vals, true, rCuller, p_relv_data_vals->size() );

	//pop seg name
	glPopName();
}

/*
 *	Recursively draw MipChunks within the Segmentation using the MipCoordinate hierarchy.
 *	Uses the OmVolumeCuller to determine the visibility of a MipChunk.  If visible, the
 *	MipChunk is either drawn or the recursive draw process is called on its children.
 */
void OmSegmentation::DrawChunkRecursive(const OmMipChunkCoord & chunkCoord, const SegmentDataSet & rRelvDataVals,
					bool testVis, const OmVolumeCuller & rCuller,
					const int numSegments )
{
	//get pointer to chunk
	shared_ptr < OmMipChunk > p_chunk = shared_ptr < OmMipChunk > ();
	GetChunk(p_chunk, chunkCoord);

	//TEST FOR CHUNK VISIBILITY IF NECESSARY
	if (testVis) {
		//(possible pre-testing optimizations)
		//check if frustum sphere contains chunk bounding sphere
		//check if frustom cone contains chunk bounding sphere
		//check if frustum contains chunk bounding sphere

		//check if frustum contains chunk
		switch (rCuller.TestChunk(*p_chunk)) {
		case VISIBILITY_NONE:	//so return from drawing
			return;

		case VISIBILITY_PARTIAL:	//continue drawing tree and continue testing children
			break;

		case VISIBILITY_FULL:	//continue drawing tree, but assume children are visible
			testVis = false;
			break;
		}
	}

	
	//TEST IF CHUNK SHOULD BE DRAWN
	//if chunk satisfies draw criteria
	//	if ( numSegments > 5 && p_chunk->DrawCheck(rCuller) ) {
	if ( p_chunk->DrawCheck(rCuller) ) {
		//intersect enabled segments with data contained segments
		SegmentDataSet direct_relevant_data_set;
		setIntersection < SEGMENT_DATA_TYPE > (rRelvDataVals, p_chunk->GetDirectDataValues(),
						       direct_relevant_data_set);

		//return if empty
		if (direct_relevant_data_set.size() == 0)
			return;

		//draw data_relevent_segments in this chunk
		DrawChunk(chunkCoord, direct_relevant_data_set, rCuller);

		//done with chunk
		return;
	}
	/*
	if( !(numSegments > 5) ) {
		SegmentDataSet direct_relevant_data_set;
		setIntersection < SEGMENT_DATA_TYPE > (rRelvDataVals, p_chunk->GetDirectDataValues(),
						       direct_relevant_data_set);

		//return if empty
		if (direct_relevant_data_set.size() == 0)
			return;

		//draw data_relevent_segments in this chunk
		DrawChunk(chunkCoord, direct_relevant_data_set, rCuller);
	} 
	*/
	////ELSE BREAK DOWN INTO CHILDREN
	//intersect enabled segments with spactially contained segments
	SegmentDataSet indirect_relevant_data_set;
	setIntersection < SEGMENT_DATA_TYPE > (rRelvDataVals, p_chunk->GetIndirectDataValues(),
						       indirect_relevant_data_set);
	
	//return if empty
	if (indirect_relevant_data_set.size() == 0)
		return;

	//for each valid child
	set < OmMipChunkCoord >::iterator itr;
	for (itr = p_chunk->GetChildrenCoordinates().begin(); itr != p_chunk->GetChildrenCoordinates().end(); itr++) {
		//draw child with only relevant segments enabled
		DrawChunkRecursive(*itr, indirect_relevant_data_set, testVis, rCuller, numSegments);
	}

}

/*
 *	MipChunk determined to be visible so draw contents depending on mode.
 */
void OmSegmentation::DrawChunk(const OmMipChunkCoord & chunkCoord, const SegmentDataSet & rRelvDataVals,
			       const OmVolumeCuller & rCuller)
{
	//debug ("genone", "OmSegmentation::DrawChunk\n");

	//get pointer to chunk
	shared_ptr < OmMipChunk > p_chunk = shared_ptr < OmMipChunk > ();
	GetChunk(p_chunk, chunkCoord);

	//draw extent
	if (rCuller.CheckDrawOption(DRAWOP_DRAW_CHUNK_EXTENT)) {
		p_chunk->DrawClippedExtent();
	}
	//if not set to render segments
	if (!rCuller.CheckDrawOption(DRAWOP_LEVEL_SEGMENT)) {
		return;
	}

	//debug ("genone", "OmSegmentation::DrawChunk (isleaf=%i)\n", p_chunk->IsLeaf());

	//if editing and chunk is leaf
	if ((OmStateManager::GetSystemMode() == EDIT_SYSTEM_MODE) && p_chunk->IsLeaf()
						&& GetVoxelizedSegmentDataValues().size()) {
		//then draw selected as voxels
		SegmentDataSet selected_relevant_data_set;
		setIntersection < SEGMENT_DATA_TYPE > (rRelvDataVals, GetVoxelizedSegmentDataValues(),
						       selected_relevant_data_set);
		DrawChunkVoxels(chunkCoord, selected_relevant_data_set, rCuller.GetDrawOptions());

		SegmentDataSet theRest;
		SegmentDataSet::iterator itr;
		for (itr = rRelvDataVals.begin(); !(itr == rRelvDataVals.end()); itr++) {
			if (GetVoxelizedSegmentDataValues().end() ==
						GetVoxelizedSegmentDataValues().find (*itr)) {
				theRest.insert (*itr);
			}
		}
		

		//draw un-selected as meshes
		SegmentDataSet unselected_relevant_data_set;
		setIntersection < SEGMENT_DATA_TYPE > (rRelvDataVals, theRest,
						       unselected_relevant_data_set);
		DrawChunkMeshes(chunkCoord, unselected_relevant_data_set, rCuller.GetDrawOptions());

	} else {
		//else draw mesh
		DrawChunkMeshes(chunkCoord, rRelvDataVals, rCuller.GetDrawOptions());
	}
}

/*
 *	Draw relavant meshes in MipChunk with specified coordinate.
 */
void OmSegmentation::DrawChunkMeshes(const OmMipChunkCoord & mipCoord, const SegmentDataSet & rRelvDataVals,
				     const OmBitfield & drawOps)
{
	mMipMeshManager.DrawMeshes(mSegmentManager, drawOps, mipCoord, rRelvDataVals);
}

/*
 *	Draw voxelated representation of the MipChunk.
 */
void OmSegmentation::DrawChunkVoxels(const OmMipChunkCoord & mipCoord, const SegmentDataSet & rRelvDataVals,
				     const OmBitfield & drawOps)
{
	//debug ("genone", "OmSegmentation::DrawChunkVoxels\n");
	mMipVoxelationManager.DrawVoxelations(mSegmentManager, mipCoord, rRelvDataVals, drawOps);
}

/*
 //get pointer to chunk
 shared_ptr<OmMipChunk> p_chunk = GetChunk(mipCoord);
 
 //push modelview matrix
 glPushMatrix();
 */

/*
 //transform into voxel space
 Vector3f translate = mNormExtent.getMin();
 Vector3f scale = Vector3f::ONE / GetDataExtent().getUnitDimensions();
 Vector3f norm_extent_scale = GetNormExtent().getDimensions();
 glTranslatefv(translate.array);
 glScalefv(norm_extent_scale.array);	//makes chunk dims = 1
 glScalefv(scale.array);	//makes chunk dims match data extent
 */

//call voxel map draw
//mChunkVoxels.DrawVoxels(rCuller, rSegmentation, rRelvDataVals);

//glPopMatrix();

#pragma mark
#pragma mark Print Methods
/////////////////////////////////
///////          Print Methods

void OmSegmentation::Print()
{
	//debug("FIXME", << "\t" << mName << " (" << mId << ")" << endl;

	if (mSegmentManager.SegmentSize()) {
		//debug("FIXME", << "\t   Segments:" << endl;
		mSegmentManager.SegmentCall(&OmSegment::Print);
	}
}
