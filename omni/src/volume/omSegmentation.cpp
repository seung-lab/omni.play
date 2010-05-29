#include "common/omCommon.h"
#include "common/omDebug.h"
#include "common/omUtility.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "mesh/meshingManager.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentColorizer.h"
#include "segment/omSegmentEditor.h"
#include "segment/omSegmentIterator.h"
#include "system/events/omProgressEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omView3dEvent.h"
#include "system/omEventManager.h"
#include "system/omGenericManager.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "volume/omMipChunk.h"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationChunkCoord.h"
#include "volume/omVolume.h"
#include "volume/omVolumeCuller.h"

#include <vtkImageData.h>
#include <QFile>
#include <QTextStream>

/////////////////////////////////
///////
///////         OmSegmentation
///////

OmSegmentation::OmSegmentation()
	: mMipVoxelationManager(this)
	, mSegmentCache(new OmSegmentCache(this))
	, mGroups(this)
{
	SetBytesPerSample(SEGMENT_DATA_BYTES_PER_SAMPLE);

        SetCacheName("OmSegmentation -> OmMipVolume");
        int chunkDim = GetChunkDimension();
        SetObjectSize(chunkDim*chunkDim*chunkDim*GetBytesPerSample());
	mMeshingMan = NULL;

        mDend = NULL;
        mDendValues = NULL;
	mDendSize = 0;
	mDendValuesSize = 0;
	mDendCount = 0;
	mDendThreshold = 0.0;
}

OmSegmentation::OmSegmentation(OmId id)
	: OmManageableObject(id)
	, mMipVoxelationManager(this)
	, mSegmentCache(new OmSegmentCache(this))
	, mGroups(this)
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

        mDend = NULL;
        mDendValues = NULL;
	mDendSize = 0;
	mDendValuesSize = 0;
	mDendCount = 0;
	mDendThreshold = 0.0;

        SetCacheName("OmSegmentation -> OmMipVolume");
        int chunkDim = GetChunkDimension();
        SetObjectSize(chunkDim*chunkDim*chunkDim*GetBytesPerSample());
}

OmSegmentation::~OmSegmentation()
{
	KillCacheThreads();

	delete mSegmentCache;

	if(mDend) {
        	free(mDend);
	}
	if(mDendValues){
        	free(mDendValues);
	}
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
	return GetVoxelValue(vox);
}

/*
 *	Convienence method that sets the data value of the voxel to
 *	any value that is mapped to the given SegmentId
 */
void OmSegmentation::SetVoxelSegmentId(const DataCoord & vox, OmId omId)
{
	//set voxel to first value in set
	SetVoxelValue(vox, omId);
}

/////////////////////////////////
///////          Build Methods

bool OmSegmentation::IsVolumeDataBuilt()
{
	return OmMipVolume::IsBuilt();
}

void OmSegmentation::BuildVolumeData()
{
	mSegmentCache->turnBatchModeOn(true);

	//build volume
	OmMipVolume::Build();

	for (int level = 0; level < GetRootMipLevel(); ++level) {

		printf("segment build mip level %d...\n", level );

		Vector3 < int >mip_coord_dims = MipLevelDimensionsInMipChunks(level);

		for (int z = 0; z < mip_coord_dims.z; ++z) {
			for (int y = 0; y < mip_coord_dims.y; ++y) {
				for (int x = 0; x < mip_coord_dims.x; ++x) {

					OmMipChunkCoord chunk_coord(level, x, y, z);

					QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
					GetChunk(p_chunk, chunk_coord);
					const OmSegIDs & r_root_indirect_data_values = p_chunk->GetDirectDataValues();

					// will already be mappped
					mSegmentCache->AddSegmentsFromChunk(r_root_indirect_data_values, chunk_coord);
				}
			}
		}
	}
	
	mSegmentCache->flushDirtySegments();
	mSegmentCache->turnBatchModeOn(false);
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
	//build chunk volume data
	OmMipVolume::BuildChunk(mipCoord);

	//get pointer to chunk
	QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
	GetChunk(p_chunk, mipCoord);

	//analyze entire chunk segmentation data
	p_chunk->RefreshDirectDataValues( mSegmentCache );

	//if root then update segment manager with spacial content information
	if (p_chunk->IsRoot()) {
		//get root spatial segs
		const OmSegIDs & data_values = p_chunk->GetDirectDataValues();

		//add to manager if data isn't already mapped
		mSegmentCache->AddSegmentsFromChunk( data_values, mipCoord);
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

		const OmSegIDs & rModifiedValues = p_chunk->GetModifiedVoxelValues();
		if (rModifiedValues.size() == 0) {
			return;
		}

		//remove mesh from cache to force it to reload
		foreach( OmSegID val, rModifiedValues ){
			OmMipMeshCoord mip_mesh_coord = OmMipMeshCoord(mipCoord, val);
			mMipMeshManager.UncacheMesh(mip_mesh_coord);
		}
	}
}

void OmSegmentation::RebuildChunk(const OmMipChunkCoord & mipCoord, const OmSegIDs & rModifiedValues)
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
	foreach( OmSegID val, rModifiedValues ){
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
	mSegmentCache->mMutex.lock();

	//get data extent (varify it is a chunk)
	int extent[6];
	pImageData->GetExtent(extent);

	//get pointer to native scalar data
	assert(pImageData->GetScalarSize() == SEGMENT_DATA_BYTES_PER_SAMPLE);
	OmSegID * p_scalar_data = static_cast<OmSegID*>( pImageData->GetScalarPointer() );

	//for all voxels in the chunk
	int x, y, z;
	for (z = extent[0]; z <= extent[1]; z++) {
		for (y = extent[2]; y <= extent[3]; y++) {
			for (x = extent[4]; x <= extent[5]; x++) {

				//if non-null segment value
				if (NULL_SEGMENT_DATA != *p_scalar_data) {
					*p_scalar_data = mSegmentCache->findRootID_noLock(*p_scalar_data);
				}
				//adv to next scalar
				++p_scalar_data;
			}
		}
	}

	mSegmentCache->mMutex.unlock();
}

/////////////////////////////////
///////          Event Handling

void OmSegmentation::SystemModeChangeEvent()
{
	//if change out of editing mode
	if (OmStateManager::GetSystemMode() != EDIT_SYSTEM_MODE &&
	    OmStateManager::GetSystemModePrev() == EDIT_SYSTEM_MODE) {
		//then build edited chunks
		OmMipVolume::Flush();
	}
}

/////////////////////////////////
///////          Segment Management

OmSegment * OmSegmentation::GetSegment(OmId id)
{
	return mSegmentCache->GetSegmentFromValue(id);
}

OmSegment* OmSegmentation::GetSegmentFromValue(OmSegID val)
{
	return mSegmentCache->GetSegmentFromValue(val);
}

OmSegment * OmSegmentation::AddSegment()
{
	OmSegment* segment = mSegmentCache->AddSegment();
	return segment;
}

bool OmSegmentation::IsSegmentValid(OmId id)
{
	return mSegmentCache->IsSegmentValid(id);
}

OmId OmSegmentation::GetNumSegments()
{
	return mSegmentCache->GetNumSegments();
}

OmId OmSegmentation::GetNumTopSegments()
{
	return mSegmentCache->GetNumTopSegments();
}

bool OmSegmentation::IsSegmentEnabled(OmId id)
{
	return mSegmentCache->isSegmentEnabled(id);
}

void OmSegmentation::SetSegmentEnabled(OmId id, bool enable)
{
	mSegmentCache->setSegmentEnabled(id, enable);
}

const OmIds & OmSegmentation::GetEnabledSegmentIds()
{
	return mSegmentCache->GetEnabledSegmentIdsRef();
}

bool OmSegmentation::IsSegmentSelected(OmId id)
{
	return mSegmentCache->isSegmentSelected(id);
}

void OmSegmentation::SetSegmentSelected(OmId id, bool selected)
{
	mSegmentCache->setSegmentSelected(id, selected);
}

void OmSegmentation::SetAllSegmentsSelected(bool selected)
{
	mSegmentCache->SetAllSelected(selected);
}

void OmSegmentation::SetAllSegmentsEnabled(bool enabled)
{
	mSegmentCache->SetAllEnabled(enabled);
}

const OmIds & OmSegmentation::GetSelectedSegmentIds()
{
	return mSegmentCache->GetSelectedSegmentIdsRef();
}

bool OmSegmentation::AreSegmentsSelected()
{
	return mSegmentCache->AreSegmentsSelected();
}

void OmSegmentation::UpdateSegmentSelections( const OmSegIDs & idsToSelect,
					      const OmSegIDs & idsToUnselect )
{
	mSegmentCache->UpdateSegmentSelections(idsToSelect, idsToUnselect );
}

/////////////////////////////////
///////          Groups
OmId OmSegmentation::AddGroup()
{
        OmSegmentIterator iter(mSegmentCache);

        iter.iterOverSelectedIDs();

	OmIds segmentsToGroup;
        OmSegment * seg = iter.getNextSegment();
        OmSegID val;
        while(NULL != seg) {
        	val = seg->getValue();
        	seg->SetImmutable(true);
                //segmentsToGroup.insert(val);
        	seg = iter.getNextSegment();
        }

	return mGroups.AddGroup(segmentsToGroup);
}




/////////////////////////////////
///////          Draw

/*
 *	Draw the entire Segmentation.  Starts the recursive processes of drawing MipChunks
 *	from the root MipChunk of the Segmentation.  Filters for relevant data values to be 
 *	drawn depending on culler draw options and passes relevant set to root chunk.
 */
void OmSegmentation::Draw(OmVolumeCuller & rCuller, OmViewGroupState * vgs)
{
	mViewGroupState = vgs;		// This is hackish because i don;t want to thread this into all the
					// recursive function calls.
	//transform to normal frame
	glPushMatrix();
	glMultMatrixf(mNormToSpaceMat.ml);

	//draw volume axis
	if (rCuller.CheckDrawOption(DRAWOP_DRAW_VOLUME_AXIS)) {
		glDrawPositiveAxis();
	}

	//return if no chunk level drawing
	if (!rCuller.CheckDrawOption(DRAWOP_LEVEL_CHUNKS)) {
		glPopMatrix();
		return;
	}
	//form culler for this volume and call draw on all volumes
	OmVolumeCuller volume_culler =
		rCuller.GetTransformedCuller(mNormToSpaceMat, mNormToSpaceInvMat);

	OmSegmentIterator iter(mSegmentCache);

	//check to filter for relevant data values
	if (rCuller.CheckDrawOption(DRAWOP_SEGMENT_FILTER_SELECTED)) {
		iter.iterOverSelectedIDs();
		
	} else if (rCuller.CheckDrawOption(DRAWOP_SEGMENT_FILTER_UNSELECTED)) {
		iter.iterOverEnabledIDs();
	}

	if( iter.empty() ){
        	glPopMatrix();
		return;
	}

	glPushName(GetId());

	//draw relevant data values starting from root chunk
	DrawChunkRecursive(RootMipChunkCoordinate(), iter, true, volume_culler );

	glPopName();

        //pop matrix
        glPopMatrix();

}



/*
 *	Recursively draw MipChunks within the Segmentation using the MipCoordinate hierarchy.
 *	Uses the OmVolumeCuller to determine the visibility of a MipChunk.  If visible, the
 *	MipChunk is either drawn or the recursive draw process is called on its children.
 */
void OmSegmentation::DrawChunkRecursive(const OmMipChunkCoord & chunkCoord, 
					OmSegmentIterator segIter,
					bool testVis, 
					OmVolumeCuller & rCuller )
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

		std::vector< OmSegment* > segmentsToDraw;

		if( !mSegmentCache->segmentListDirectCacheHasCoord( chunkCoord ) ){
			const OmSegIDs & chunkValues =  p_chunk->GetDirectDataValues();
			OmSegment * seg = segIter.getNextSegment();
			OmSegID val;
			while( NULL != seg ){
				val = seg->getValue();
				if( chunkValues.contains( val ) ){
					segmentsToDraw.push_back(seg);
				}
				
				seg = segIter.getNextSegment();
			}
			mSegmentCache->setSegmentListDirectCache( chunkCoord, segmentsToDraw );
			//printf("segmentsToDraw=%i\n", segmentsToDraw.size());
		}

		return DrawChunk(p_chunk, chunkCoord, mSegmentCache->getSegmentListDirectCache( chunkCoord ), rCuller);
	}

	// ELSE BREAK DOWN INTO CHILDREN

	const set<OmMipChunkCoord> & coords = p_chunk->GetChildrenCoordinates();
	std::set<OmMipChunkCoord>::const_iterator iter;
	for( iter = coords.begin(); iter != coords.end(); ++iter ){
		DrawChunkRecursive(*iter, segIter, testVis, rCuller);
	}
}

/*
 *	MipChunk determined to be visible so draw contents depending on mode.
 */
void OmSegmentation::DrawChunk(QExplicitlySharedDataPointer < OmMipChunk > p_chunk,
			       const OmMipChunkCoord & chunkCoord,	
			       const OmSegPtrs & segmentsToDraw,
			       OmVolumeCuller & rCuller)
{
	if( segmentsToDraw.empty() ){
		return;
	}

	//draw extent
	if (rCuller.CheckDrawOption(DRAWOP_DRAW_CHUNK_EXTENT)) {
		p_chunk->DrawClippedExtent();
	}

	//if not set to render segments
	if (!rCuller.CheckDrawOption(DRAWOP_LEVEL_SEGMENT)) {
		return;
	}

	mMipMeshManager.DrawMeshes(rCuller.GetDrawOptions(), 
				   chunkCoord, 
				   segmentsToDraw, mViewGroupState);
}

/*
 *	Draw voxelated representation of the MipChunk.
 */
void OmSegmentation::DrawChunkVoxels(const OmMipChunkCoord & mipCoord, const OmSegIDs & rRelvDataVals,
				     const OmBitfield & drawOps)
{
	mMipVoxelationManager.DrawVoxelations(mSegmentCache, mipCoord, rRelvDataVals, drawOps);
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
	mSegmentCache->flushDirtySegments();
}

void OmSegmentation::FlushDend()
{
	QString dendStr = QString("%1/dend")
		.arg(GetDirectoryPath());
	QString dendValStr = QString("%1/dendValues")
		.arg(GetDirectoryPath());
	OmDataPath path;
	
	path.setPathQstr(dendStr);
	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite(path, mDendSize, mDend);

	path.setPathQstr(dendValStr);
	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite(path, mDendValuesSize, mDendValues);
}

void OmSegmentation::ReloadDendrogram()
{
	mSegmentCache->resetGlobalThreshold(mDendThreshold);
}

void OmSegmentation::JoinTheseSegments( const OmIds & segmentIds)
{
	mSegmentCache->JoinTheseSegments(segmentIds);
}

void OmSegmentation::UnJoinTheseSegments( const OmIds & segmentIds)
{
	mSegmentCache->UnJoinTheseSegments(segmentIds);
}

void OmSegmentation::SetDendThreshold( float t ){
	mDendThreshold = t;
}

void OmSegmentation::SetDendThresholdAndReload( const float t ){
	if( t == mDendThreshold ){
		return;
	}
	SetDendThreshold(t);
	ReloadDendrogram();
}

