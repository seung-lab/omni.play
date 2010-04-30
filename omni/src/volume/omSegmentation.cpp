#include "omSegmentation.h"
#include "omVolume.h"
#include "omMipChunk.h"
#include "omVolumeCuller.h"

#include "segment/omSegmentEditor.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "system/omEventManager.h"
#include "system/events/omProgressEvent.h"
#include "system/events/omView3dEvent.h"
#include "system/events/omSegmentEvent.h"
#include "common/omUtility.h"

#include <vtkImageData.h>

#include "common/omDebug.h"
#include <QFile>
#include <QTextStream>

/////////////////////////////////
///////
///////         OmSegmentation
///////

OmSegmentation::OmSegmentation()
	: mMipVoxelationManager(this), mSegmentCache(this)
{
	SetBytesPerSample(SEGMENT_DATA_BYTES_PER_SAMPLE);

        //SetCacheName("OmMipMeshManager");

	mMeshingMan = NULL;
}

OmSegmentation::OmSegmentation(OmId id)
	: OmManageableObject(id), mMipVoxelationManager(this), mSegmentCache(this)
{
	//set manageable object name
	SetName( QString("segmentation%1").arg(id));

	//set permenant directory name
	SetDirectoryPath( QString("segmentations/%1/").arg(GetName()) );

	mMipMeshManager.SetDirectoryPath( GetDirectoryPath() );

	//segmenations has SEGMENT_DATA_BYTES_PER_SAMPLE bytes per sample
	SetBytesPerSample(SEGMENT_DATA_BYTES_PER_SAMPLE);

	//subsample segmentation data using mode
	SetSubsampleMode(SUBSAMPLE_NONE);

	//uses meta data
	mStoreChunkMetaData = true;

	//build blank data
	BuildVolumeData();

	mMeshingMan = NULL;
}

OmSegmentation::~OmSegmentation()
{
	KillCacheThreads();
}

/////////////////////////////////
///////          Data Mapping

/*
 *	Get the Segment OmId mapped to the given data value.
 */
OmId OmSegmentation::GetSegmentIdMappedToValue(SEGMENT_DATA_TYPE value)
{
	OmSegment * seg = mSegmentCache.GetSegmentFromValue(value);
	if (!seg) {
		return 0;
	}
	return seg->GetId();
}

/*
 *	Get the set of data values that map to the given Segment OmId.
 */
SegmentDataSet OmSegmentation::GetValuesMappedToSegmentId(OmId id)
{
	return mSegmentCache.GetSegmentFromID(id)->getValues();
}

SEGMENT_DATA_TYPE OmSegmentation::GetValueMappedToSegmentId(OmId id)
{
	return mSegmentCache.GetSegmentFromID(id)->getValue();
}

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

/////////////////////////////////
///////          Build Methods

bool OmSegmentation::IsVolumeDataBuilt()
{
	return OmMipVolume::IsBuilt();
}

void OmSegmentation::BuildVolumeData()
{
	//build volume
	OmMipVolume::Build();

	// skip root (already done)
	for (int level = 0; level < GetRootMipLevel(); ++level) {

		Vector3 < int >mip_coord_dims = MipLevelDimensionsInMipChunks(level);

		for (int z = 0; z < mip_coord_dims.z; ++z) {
			for (int y = 0; y < mip_coord_dims.y; ++y) {
				for (int x = 0; x < mip_coord_dims.x; ++x) {

					OmMipChunkCoord chunk_coord(level, x, y, z);

					QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
					GetChunk(p_chunk, chunk_coord);
					const SegmentDataSet & r_root_indirect_data_values = p_chunk->GetDirectDataValues();

					// will already be mappped
					mSegmentCache.AddSegmentsFromChunk(r_root_indirect_data_values, chunk_coord);
				}
			}
		}
	}

	//seg change event
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
}

/*
 *	Build all meshes in all chunks of the MipVolume.
 */
void OmSegmentation::BuildMeshData()
{
	if (!IsVolumeDataBuilt()) {
		throw OmAccessException("Segmentation volume data must be built before mesh data: " +
					GetName().toStdString() );
	}

	//build all levels
	BuildMeshDataInternal();
}

/**
 * Produce the plan file.
 */
void OmSegmentation::BuildMeshDataPlan(const QString & planFile)
{
        if (!IsVolumeDataBuilt())
                throw OmAccessException(string("Segmentation volume data must be built before mesh data: ") +
                                        GetName().toStdString() );

    	QFile file(planFile);
    	if ( !file.open(QIODevice::WriteOnly)) {
		throw (false && "couldn't open the plan file, check to make sure you have write permission");
	}
        QTextStream stream(&file);

        //for each level
        for (int level = GetRootMipLevel(); level >= 0; --level) {

                //dim of leaf coords
                Vector3 < int >mip_coord_dims = MipLevelDimensionsInMipChunks(level);

                //for all coords of level
                for (int z = mip_coord_dims.z; z >= 0; --z) {
                        for (int y = mip_coord_dims.y; y >= 0; --y) {
                                for (int x = mip_coord_dims.x; x >= 0; --x) {

					stream << "meshchunk:" << GetId() << ":" << level << ":" << x << "," << y << "," << z << endl;
                                }
			}
		}
        }

        file.close();
}


/**
 * Build the meshes for a single chunk.
 */
void OmSegmentation::BuildMeshChunk(int level, int x, int y, int z, int numThreads )
{
	MeshingManager* meshingMan = new MeshingManager( GetId(), &mMipMeshManager );
	OmMipChunkCoord chunk_coord(level, x, y, z);

	meshingMan->addToQueue( chunk_coord );
	if( numThreads > 0 ){
		meshingMan->setNumThreads( numThreads );
	}
	meshingMan->start();
	meshingMan->wait();
	delete(meshingMan);
}

void OmSegmentation::BuildMeshDataInternal()
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
	delete(meshingMan);
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
	QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
	GetChunk(p_chunk, mipCoord);

	//analyze entire chunk segmentation data
	p_chunk->RefreshDirectDataValues();
	p_chunk->RefreshIndirectDataValues();

	//if root then update segment manager with spacial content information
	if (p_chunk->IsRoot()) {
		//get root spatial segs
		const SegmentDataSet & r_root_indirect_data_values = p_chunk->GetIndirectDataValues();

		//add to manager if data isn't already mapped
		mSegmentCache.AddSegmentsFromChunk(r_root_indirect_data_values, mipCoord);
	}

	//rebuild mesh data only if entire volume data has been built as well
	if (IsVolumeDataBuilt() ) {
		MeshingManager* meshingMan = new MeshingManager( GetId(), &mMipMeshManager );
		meshingMan->setToOnlyMeshModifiedValues();
		meshingMan->addToQueue( mipCoord );
		meshingMan->start();
		meshingMan->wait();

		QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
		GetChunk(p_chunk, mipCoord);

		const SegmentDataSet & rModifiedValues = p_chunk->GetModifiedVoxelValues();
		if (rModifiedValues.size() == 0) {
			return;
		}

		//remove mesh from cache to force it to reload
		foreach( SEGMENT_DATA_TYPE val, rModifiedValues ){
			OmMipMeshCoord mip_mesh_coord = OmMipMeshCoord(mipCoord, val);
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
		MeshingManager* meshingMan = new MeshingManager( GetId(), &mMipMeshManager );
		meshingMan->addToQueue( mipCoord );
		meshingMan->start();
		meshingMan->wait();
	}

	//remove mesh from cache to force it to reload
	foreach( SEGMENT_DATA_TYPE val, rModifiedValues ){
		OmMipMeshCoord mip_mesh_coord = OmMipMeshCoord(mipCoord, val);
		mMipMeshManager.UncacheMesh(mip_mesh_coord);
	}

	//call redraw to force mesh to reload
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}

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
	for (z = extent[0]; z <= extent[1]; z++) {
		for (y = extent[2]; y <= extent[3]; y++) {
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
	}
}

/////////////////////////////////
///////          Event Handling

void OmSegmentation::SystemModeChangeEvent()
{
	//if change out of editing mode
	if (OmStateManager::GetSystemMode() != EDIT_SYSTEM_MODE) {
		//then build edited chunks
		OmMipVolume::Flush();
	}
}

/////////////////////////////////
///////          Segment Management

OmSegment * OmSegmentation::GetSegment(OmId id)
{
	return mSegmentCache.GetSegmentFromID(id);
}

OmSegment* OmSegmentation::GetSegmentFromValue(SEGMENT_DATA_TYPE val)
{
	return mSegmentCache.GetSegmentFromValue(val);
}

OmSegment * OmSegmentation::AddSegment()
{
	OmSegment* segment = mSegmentCache.AddSegment();
	return segment;
}

bool OmSegmentation::IsSegmentValid(OmId id)
{
	return mSegmentCache.IsSegmentValid(id);
}

OmId OmSegmentation::GetNumSegments()
{
	return mSegmentCache.GetNumSegments();
}

OmId OmSegmentation::GetNumTopSegments()
{
	return mSegmentCache.GetNumTopSegments();
}

bool OmSegmentation::IsSegmentEnabled(OmId id)
{
	return mSegmentCache.GetSegmentFromID(id)->IsEnabled();
}

void OmSegmentation::SetSegmentEnabled(OmId id, bool enable)
{
	mSegmentCache.GetSegmentFromID(id)->SetEnabled(enable);
}

const OmIds & OmSegmentation::GetEnabledSegmentIds()
{
	return mSegmentCache.GetEnabledSegmentIdsRef();
}

bool OmSegmentation::IsSegmentSelected(OmId id)
{
	return mSegmentCache.GetSegmentFromID(id)->IsSelected();
}

void OmSegmentation::SetSegmentSelected(OmId id, bool selected)
{
	mSegmentCache.GetSegmentFromID(id)->SetSelected(selected);
}

void OmSegmentation::SetAllSegmentsSelected(bool selected)
{
	mSegmentCache.SetAllSelected(selected);
}

void OmSegmentation::SetAllSegmentsEnabled(bool enabled)
{
	mSegmentCache.SetAllEnabled(enabled);
}

const OmIds & OmSegmentation::GetSelectedSegmentIds()
{
	return mSegmentCache.GetSelectedSegmentIdsRef();
}

bool OmSegmentation::AreSegmentsSelected()
{
	if( 0 == mSegmentCache.numberOfSelectedSegments() ){
		return false;
	}

	return true;
}

/////////////////////////////////
///////          Draw

/*
 *	Draw the entire Segmentation.  Starts the recursive processes of drawing MipChunks
 *	from the root MipChunk of the Segmentation.  Filters for relevant data values to be 
 *	drawn depending on culler draw options and passes relevant set to root chunk.
 */
void OmSegmentation::Draw(OmVolumeCuller & rCuller)
{
	SegmentDataSet values;

	//check to filter for relevant data values
	if (rCuller.CheckDrawOption(DRAWOP_SEGMENT_FILTER_SELECTED)) {
		values = mSegmentCache.GetSelectedSegmentValues();
		
	} else if (rCuller.CheckDrawOption(DRAWOP_SEGMENT_FILTER_UNSELECTED)) {
		values = mSegmentCache.GetEnabledSegmentValues(); 
	}

	if (0 == values.size() ){
		return;
	}

	glPushName(GetId());

	//draw relevant data values starting from root chunk
	DrawChunkRecursive(RootMipChunkCoordinate(), values, true, rCuller, values.size() );

	glPopName();
}

/*
 *	Recursively draw MipChunks within the Segmentation using the MipCoordinate hierarchy.
 *	Uses the OmVolumeCuller to determine the visibility of a MipChunk.  If visible, the
 *	MipChunk is either drawn or the recursive draw process is called on its children.
 */
void OmSegmentation::DrawChunkRecursive(const OmMipChunkCoord & chunkCoord, 
					const SegmentDataSet & rRelvDataVals,
					bool testVis, 
					OmVolumeCuller & rCuller,
					const int numSegments )
{
	// get pointer to chunk
	QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
	GetChunk(p_chunk, chunkCoord);

	// TEST FOR CHUNK VISIBILITY IF NECESSARY
	if (testVis) {
		//check if frustum contains chunk
		switch (rCuller.TestChunk(*p_chunk)) {
		case VISIBILITY_NONE:
			return;

		case VISIBILITY_PARTIAL:  //continue drawing tree and continue testing children
			break;

		case VISIBILITY_FULL:	  //continue drawing tree, but assume children are visible
			testVis = false;
			break;
		}
	}
	
	// TEST IF CHUNK SHOULD BE DRAWN
	// if chunk satisfies draw criteria
	if ( p_chunk->DrawCheck(rCuller) ) {

		//intersect enabled segments with data contained segments
		SegmentDataSet direct_relevant_data_set = rRelvDataVals;
		direct_relevant_data_set.intersect( p_chunk->GetDirectDataValues() );

		//return if empty
		if (direct_relevant_data_set.size() == 0)
			return;

		//draw data_relevent_segments in this chunk
		DrawChunk(chunkCoord, direct_relevant_data_set, rCuller);

		//done with chunk
		return;
	}

	// ELSE BREAK DOWN INTO CHILDREN
	// intersect enabled segments with spactially contained segments
	SegmentDataSet indirect_relevant_data_set = rRelvDataVals;
	indirect_relevant_data_set.intersect( p_chunk->GetIndirectDataValues() );

	if (indirect_relevant_data_set.size() == 0){ 
		return;
	}

	//for each valid child
	foreach( OmMipChunkCoord coord, p_chunk->GetChildrenCoordinates() ){
		//draw child with only relevant segments enabled
		DrawChunkRecursive(coord, indirect_relevant_data_set, testVis, rCuller, numSegments);
	}
}

/*
 *	MipChunk determined to be visible so draw contents depending on mode.
 */
void OmSegmentation::DrawChunk(const OmMipChunkCoord & chunkCoord, 
			       const SegmentDataSet & rRelvDataVals,
			       OmVolumeCuller & rCuller)
{
	QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
	GetChunk(p_chunk, chunkCoord);

	//draw extent
	if (rCuller.CheckDrawOption(DRAWOP_DRAW_CHUNK_EXTENT)) {
		p_chunk->DrawClippedExtent();
	}

	//if not set to render segments
	if (!rCuller.CheckDrawOption(DRAWOP_LEVEL_SEGMENT)) {
		return;
	}

	mMipMeshManager.DrawMeshes(&mSegmentCache, 
				   rCuller.GetDrawOptions(), 
				   chunkCoord, 
				   rRelvDataVals);
}

/*
 *	Draw voxelated representation of the MipChunk.
 */
void OmSegmentation::DrawChunkVoxels(const OmMipChunkCoord & mipCoord, const SegmentDataSet & rRelvDataVals,
				     const OmBitfield & drawOps)
{
	mMipVoxelationManager.DrawVoxelations(&mSegmentCache, mipCoord, rRelvDataVals, drawOps);
}

/**
 * Enqueue chunk coord to build
 */
void OmSegmentation::QueueUpMeshChunk(OmSegmentationChunkCoord chunk_coord )
{
	if( NULL == mMeshingMan ){
		mMeshingMan = new MeshingManager( GetId(), &mMipMeshManager );
	}

	mMeshingMan->addToQueue( chunk_coord );
}

void OmSegmentation::RunMeshQueue()
{
	if(  NULL == mMeshingMan ){
		return;
	}

	mMeshingMan->start();
	mMeshingMan->wait();
	delete(mMeshingMan);
	mMeshingMan = NULL;
}

void OmSegmentation::KillCacheThreads()
{
	mMipVoxelationManager.KillFetchThread();
	mMipMeshManager.KillFetchThread();
	this->KillFetchThread();
}

void OmSegmentation::FlushDirtySegments()
{
	mSegmentCache.flushDirtySegments();
}

QList< OmMipChunkCoord > OmSegmentation::getMipCoordsInFrustrum(OmVolumeCuller & rCuller)
{
	QList< OmMipChunkCoord > coords;

	for (int level = 0; level <= GetRootMipLevel(); ++level) {

		Vector3 < int >mip_coord_dims = MipLevelDimensionsInMipChunks(level);

		for (int z = 0; z < mip_coord_dims.z; ++z) {
			for (int y = 0; y < mip_coord_dims.y; ++y) {
				for (int x = 0; x < mip_coord_dims.x; ++x) {
					OmMipChunkCoord coord(level, x, y, z);
					NormBbox normExtent = MipCoordToNormBbox(coord);
					
					switch (rCuller.VisibilityTestNormBbox(normExtent)) {
					case VISIBILITY_NONE:
						break;
						
					case VISIBILITY_PARTIAL:
						coords.append( coord );
						break;
						
					case VISIBILITY_FULL:
						// can optimize here
						// testVis = false;
						coords.append( coord );
						break;
					}
				}
			}
		}
	}
	
	return coords;
}
