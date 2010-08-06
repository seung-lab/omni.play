#include "common/omCommon.h"
#include "common/omDebug.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "mesh/ziMesher.h"
#include "segment/actions/omSegmentEditor.h"
#include "segment/actions/segment/omSegmentGroupAction.h"
#include "segment/actions/segment/omSegmentValidateAction.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentColorizer.h"
#include "segment/omSegmentIterator.h"
#include "system/cache/omMipVolumeCache.h"
#include "system/events/omProgressEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omView3dEvent.h"
#include "system/omEventManager.h"
#include "system/omGenericManager.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "utility/omTimer.h"
#include "volume/omMipChunk.h"
#include "volume/omMipThreadManager.h"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationChunkCoord.h"
#include "volume/omThreadChunkLevel.h"
#include "volume/omVolume.h"
#include "volume/omVolumeCuller.h"

#include <vtkImageData.h>
#include <QFile>
#include <QTextStream>


/////////////////////////////////
///////         OmSegmentation

OmSegmentation::OmSegmentation()
	: mSegmentCache(new OmSegmentCache(this))
	, mGroups(this)
{
	SetBytesPerSample(SEGMENT_DATA_BYTES_PER_SAMPLE);
}

OmSegmentation::OmSegmentation(OmId id)
	: OmManageableObject(id)
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

	//uses meta data
	mStoreChunkMetaData = true;

	//build blank data
	BuildVolumeData();
}

OmSegmentation::~OmSegmentation()
{
	delete mSegmentCache;
}

/////////////////////////////////
///////          Data Accessors

/*
 *	Overridden so as to update MipVoxelationManager
 */
void OmSegmentation::SetVoxelValue(const DataCoord & rVox, uint32_t val)
{
	OmMipVolume::SetVoxelValue(rVox, val);
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
	OmDataPath dataset = OmDataPath("main");
	OmMipVolume::Build(dataset);

	mSegmentCache->flushDirtySegments();
	mSegmentCache->turnBatchModeOn(false);
}

bool OmSegmentation::BuildThreadedVolume()
{
	OmTimer vol_timer;

	if (isDebugCategoryEnabled("perftest")){
		//timer start
		vol_timer.start();
	}

	if (!OmMipVolume::BuildThreadedVolume()){
		return false;
	}

	if (!BuildThreadedSegmentation()){
		return false;
	}

	if (isDebugCategoryEnabled("perftest")){
		//timer stop
		vol_timer.stop();
		printf("OmSegmentation::BuildThreadedVolume() done : %.6f secs\n",vol_timer.s_elapsed());
	}

	return true;
}

bool OmSegmentation::BuildThreadedSegmentation()
{

	//debug("FIXME", << "OmMipVolume::BuildThreadedVolume()" << endl;
	//init progress bar
	OmEventManager::
	    PostEvent(new
		      OmProgressEvent(OmProgressEvent::PROGRESS_SHOW, string("Building volume...               "), 0,
				      MipChunksInVolume()));

	OmTimer vol_timer;

	if (isDebugCategoryEnabled("perftest")){
       		//timer start
       		vol_timer.start();
	}

	printf("Refreshing direct data values...\n");

	//initLevel doesn't matter, just set to 0
	OmMipThreadManager *mipThreadManager = new OmMipThreadManager(this,OmMipThread::MIP_CHUNK,0,false);
	mipThreadManager->SpawnThreads(MipChunksInVolume());
	mipThreadManager->run();
	mipThreadManager->wait();
	mipThreadManager->StopThreads();
	delete mipThreadManager;

	//flush cache so that all thread chunks are flushed to disk
	printf("Flushing mipchunks...\n");
	Flush();
	printf("done\n");

	if (isDebugCategoryEnabled("perftest")){

		//timer end
		vol_timer.stop();
		printf("OmSegmentation::BuildThreadedSegmentation() done : %.6f secs\n",vol_timer.s_elapsed());

	}

	//hide progress bar
	OmEventManager::PostEvent(new OmProgressEvent(OmProgressEvent::PROGRESS_HIDE));
	return true;

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
        for (int level = GetRootMipLevel(); level >= 0; level--) {

                //dim of leaf coords
                Vector3 < int >mip_coord_dims = MipLevelDimensionsInMipChunks(level);

                //for all coords of level
                for (int z = mip_coord_dims.z-1; z >= 0; --z) {
                        for (int y = mip_coord_dims.y-1; y >= 0; --y) {
                                for (int x = mip_coord_dims.x-1; x >= 0; --x) {

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
void OmSegmentation::BuildMeshChunk(int,int,int,int,int) //int level, int x, int y, int z, int numThreads )
{
	assert(0 && "switch to ziMesher");
	/*
	MeshingManager* meshingMan = new MeshingManager( GetId(), &mMipMeshManager );
	OmMipChunkCoord chunk_coord(level, x, y, z);

	meshingMan->addToQueue( chunk_coord );
	if( numThreads > 0 ){
		meshingMan->setNumThreads( numThreads );
	}
	meshingMan->start();
	meshingMan->wait();
	delete(meshingMan);
	*/
}

void OmSegmentation::BuildMeshDataInternal()
{
	ziMesher mesher(GetId(), &mMipMeshManager, GetRootMipLevel());
	Vector3<int> mc = MipLevelDimensionsInMipChunks(0);

	for (int z = 0; z < mc.z; ++z) {
		for (int y = 0; y < mc.y; ++y) {
			for (int x = 0; x < mc.x; ++x) {
				OmMipChunkCoord chunk_coord(0, x, y, z);
				mesher.addChunkCoord(chunk_coord);
			}
		}
	}

	mesher.mesh();
}

/*
 *	Overridden BuildChunk method so that the mesh data for a chunk will
 *	also be rebuilt if needed, and segments are added from the chunk.
 */
void OmSegmentation::BuildChunk(const OmMipChunkCoord & mipCoord, bool remesh)
{
	OmMipChunkPtr p_chunk;
	GetChunk(p_chunk, mipCoord);

	const bool isMIPzero = p_chunk->IsLeaf();

	// refresh values even if not MIP 0
	boost::unordered_map< OmSegID, unsigned int> * sizes = p_chunk->RefreshDirectDataValues( isMIPzero );

	if(isMIPzero){
		const OmSegIDsSet & data_values = p_chunk->GetDirectDataValues();
		mSegmentCache->AddSegmentsFromChunk( data_values, mipCoord, sizes);
		if(remesh) {
                	ziMesher mesher(GetId(), &mMipMeshManager, GetRootMipLevel());
                	mesher.addChunkCoord(mipCoord);
                	mesher.mesh();
		}
	}

	delete sizes;
}

void OmSegmentation::RebuildChunk(const OmMipChunkCoord & mipCoord, const OmSegIDsSet & rModifiedValues)
{

	//build chunk volume data and analyze data
	OmMipVolume::BuildChunk(mipCoord);

	//rebuild mesh data only if entire volume data has been built
	if (IsVolumeDataBuilt()) {
		assert(0 && "switch to ziMesher");
		/*
		  MeshingManager* meshingMan = new MeshingManager( GetId(), &mMipMeshManager );
		  meshingMan->addToQueue( mipCoord );
		  meshingMan->start();
		  meshingMan->wait();
		*/
	}

	//remove mesh from cache to force it to reload
	foreach( const OmSegID & val, rModifiedValues ){
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
void OmSegmentation::ExportDataFilter(OmDataWrapperPtr data)
{
	mSegmentCache->ExportDataFilter(data->getVTKPtr());
}

/////////////////////////////////
///////          Groups
void OmSegmentation::SetGroup(const OmSegIDsSet & set, OmSegIDRootType type, OmGroupName name)
{

	bool valid;
	if(VALIDROOT == type) {
		valid = true;
	} else if(NOTVALIDROOT == type) {
		valid = false;
	} else if(GROUPROOT == type) {
		//mGroups.SetGroup(set, name);
		(new OmSegmentGroupAction(GetId(), set, name, true))->Run();
		return;
	}

        OmSegmentIterator iter(mSegmentCache);
        iter.iterOverSegmentIDs(set);

        OmSegment * seg = iter.getNextSegment();
	OmSegIDsSet newSet;
        while(NULL != seg) {
		newSet.insert(seg->getValue());
                seg = iter.getNextSegment();
        }

	(new OmSegmentValidateAction(GetId(), newSet, valid))->Run();
}

void OmSegmentation::UnsetGroup(const OmSegIDsSet & set, OmSegIDRootType type, OmGroupName name)
{

        if(GROUPROOT == type) {
                return mGroups.UnsetGroup(set, name);
		(new OmSegmentGroupAction(GetId(), set, name, false))->Run();
        } else {
		assert(0 && "only unset regular groups");
	}
}


void OmSegmentation::DeleteGroup(OmGroupID)
{
	printf("FIXME delete group not supported\n");
}


/*
 *	Draw voxelated representation of the MipChunk.
 */
void OmSegmentation::DrawChunkVoxels(const OmMipChunkCoord & //mipCoord
				     , const OmSegIDsSet & //rRelvDataVals
				     , const OmBitfield & //drawOps
				     )
{
	assert(0);
	//mMipVoxelationManager.DrawVoxelations(mSegmentCache, mipCoord, rRelvDataVals, drawOps);
}

/**
 * Enqueue chunk coord to build
 */
void OmSegmentation::QueueUpMeshChunk(OmSegmentationChunkCoord /*chunk_coord*/ )
{
	assert(0 && "switch to ziMesher");
	/*
	if( NULL == mMeshingMan ){
		mMeshingMan = new MeshingManager( GetId(), &mMipMeshManager );
	}

	mMeshingMan->addToQueue( chunk_coord );
	*/
}

void OmSegmentation::RunMeshQueue()
{
	assert(0 && "switch to ziMesher");

	/*
	if(  NULL == mMeshingMan ){
		return;
	}

	mMeshingMan->start();
	mMeshingMan->wait();
	delete(mMeshingMan);
	mMeshingMan = NULL;
	*/
}

void OmSegmentation::FlushDirtySegments()
{
	mSegmentCache->flushDirtySegments();
}

void OmSegmentation::FlushDend()
{
	mst.FlushDend(this);
}

void OmSegmentation::FlushDendUserEdges()
{
	mst.FlushDendUserEdges(this);
}

void OmSegmentation::SetDendThreshold( float t )
{
	if( t == mst.mDendThreshold ){
		return;
	}
	mst.mDendThreshold = t;
	mSegmentCache->refreshTree();
}

void OmSegmentation::CloseDownThreads()
{
	mMipMeshManager.CloseDownThreads();
	mDataCache->closeDownThreads();
}
