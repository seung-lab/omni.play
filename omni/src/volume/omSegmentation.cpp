#include "common/omCommon.h"
#include "common/omDebug.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
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
#include "segment/omSegmentLists.hpp"
#include "system/cache/omMipVolumeCache.h"
#include "system/events/omProgressEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omView3dEvent.h"
#include "system/omEventManager.h"
#include "system/omGenericManager.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "utility/omTimer.h"
#include "volume/build/omVolumeImporter.hpp"
#include "volume/omMipChunk.h"
#include "volume/omMipThreadManager.h"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationChunkCoord.h"
#include "volume/omThreadChunkLevel.h"
#include "volume/omVolume.h"
#include "volume/omVolumeCuller.h"
#include "volume/omVolumeData.hpp"

#include <vtkImageData.h>
#include <QFile>
#include <QTextStream>

// used by OmDataArchiveProject
OmSegmentation::OmSegmentation()
	: mVolData(new OmVolumeData())
	, mSegmentCache(new OmSegmentCache(this))
	, mSegmentLists(new OmSegmentLists())
	, mGroups(this)
{
}

// used by OmGenericManager
OmSegmentation::OmSegmentation(OmId id)
	: OmManageableObject(id)
	, mVolData(new OmVolumeData())
	, mSegmentCache(new OmSegmentCache(this))
	, mSegmentLists(new OmSegmentLists())
  	, mGroups(this)
{
	mMipMeshManager.SetDirectoryPath(QString::fromStdString(GetDirectoryPath()));

	//uses meta data
	mStoreChunkMetaData = true;

	//build blank data
	BuildVolumeData();

	mSegmentCache->refreshTree();
}

OmSegmentation::~OmSegmentation()
{
}

boost::shared_ptr<OmVolumeData> OmSegmentation::getVolData() {
	return mVolData;
}

std::string OmSegmentation::GetName(){
	return "segmentation" +  boost::lexical_cast<std::string>(GetId());
}

std::string OmSegmentation::GetDirectoryPath() {
	return OmDataPaths::getDirectoryPath(this);
}

/////////////////////////////////
///////          Build Methods

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
					GetName());
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
                                        GetName());

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
	OmMipChunkPtr chunk;
	GetChunk(chunk, mipCoord);

	const bool isMIPzero = chunk->IsLeaf();

	if(isMIPzero){
		/*
		mSegmentCache->
			AddSegmentsFromChunk(chunk->GetDirectDataValues(),
					     mipCoord,
					     chunk->RefreshDirectDataValues(true),
					     chunk->GetDirectDataBounds());
		*/

		chunk->RefreshDirectDataValues(true, mSegmentCache);

		if(remesh) {
                	ziMesher mesher(GetId(), &mMipMeshManager, GetRootMipLevel());
                	mesher.addChunkCoord(mipCoord);
                	mesher.mesh();
		}
	} else {
		chunk->RefreshDirectDataValues(false, mSegmentCache);
	}
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
///////          Groups
void OmSegmentation::SetGroup(const OmSegIDsSet & set, OmSegIDRootType type, OmGroupName name)
{
	bool valid;
	if(VALIDROOT == type) {
		valid = true;
	} else if(NOTVALIDROOT == type) {
		valid = false;
	} else if(GROUPROOT == type) {
		(new OmSegmentGroupAction(GetId(), set, name, true))->Run();
		return;
	}

        OmSegmentIterator iter(mSegmentCache);
        iter.iterOverSegmentIDs(set);

        OmSegment * seg = iter.getNextSegment();
	OmSegIDsSet newSet;
        while(NULL != seg) {
		newSet.insert(seg->value);
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

Vector3<int> OmSegmentation::FindCenterOfSelectedSegments()
{
	DataBbox box;
	bool found = false;

	OmSegmentIterator iter(mSegmentCache);
	iter.iterOverSelectedIDs();

	unsigned int counter = 0;
	const int level = 0;

	OmSegment * seg = iter.getNextSegment();
	while(NULL != seg) {

		if(seg->getBounds().isEmpty()) {
			Vector3i mip_coord_dims = MipLevelDimensionsInMipChunks(level);
			for (int z = 0; z < mip_coord_dims.z; ++z) {
				for (int y = 0; y < mip_coord_dims.y; ++y) {
					for (int x = 0; x < mip_coord_dims.x; ++x) {
						OmMipChunkCoord chunk_coord(level, x, y, z);
						OmMipChunkPtr p_chunk;
						GetChunk(p_chunk, chunk_coord);

						const OmSegIDsSet & data_values = p_chunk->GetDirectDataValues();
						if(0 != data_values.count(seg->value)) {

							if(!found) {
								found = true;
								box = p_chunk->GetExtent();
							} else {
								box = DataBbox(box, p_chunk->GetExtent());
								counter++;
							}
						}
					}
				}
			}
		} else {
			if(!found) {
				found = true;
				box = seg->getBounds();
			} else {
				box.merge(seg->getBounds());
				counter++;
			}
		}

		seg = iter.getNextSegment();
		if(counter > 5000) {
			break;
		}
	}

	if(!found) {
		return Vector3<int> (0,0,0);
		//assert(0 && "segments must be selected before calling this function");
	}
	return (box.getMin() + box.getMax()) / 2;
}


bool OmSegmentation::ImportSourceData(OmDataPath & dataset)
{
	OmVolumeImporter<OmSegmentation> importer(this);
	return importer.import(dataset);
}

void OmSegmentation::loadVolData()
{
	mVolData->load(this);
}
