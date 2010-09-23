#include "system/omGroups.h"
#include "utility/OmThreadPool.hpp"
#include "mesh/omMipMesh.h"
#include "common/omCommon.h"
#include "common/omDebug.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omMST.h"
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
#include "system/events/omSegmentEvent.h"
#include "system/events/omView3dEvent.h"
#include "system/omEventManager.h"
#include "system/omGenericManager.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "utility/omTimer.h"
#include "volume/build/omVolumeImporter.hpp"
#include "volume/omMipChunk.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "volume/omVolumeCuller.h"
#include "volume/omVolumeData.hpp"

#include <zi/threads>

#include <QFile>
#include <QTextStream>

// used by OmDataArchiveProject
OmSegmentation::OmSegmentation()
	: mDataCache(new OmMipVolumeCache(this))
	, mVolData(new OmVolumeData())
	, mSegmentCache(new OmSegmentCache(this))
	, mSegmentLists(new OmSegmentLists())
	, mGroups(boost::make_shared<OmGroups>(this))
	, mst_(new OmMST())
{
}

// used by OmGenericManager
OmSegmentation::OmSegmentation(OmId id)
	: OmManageableObject(id)
	, mDataCache(new OmMipVolumeCache(this))
	, mVolData(new OmVolumeData())
	, mSegmentCache(new OmSegmentCache(this))
	, mSegmentLists(new OmSegmentLists())
	, mGroups(boost::make_shared<OmGroups>(this))
	, mst_(new OmMST())
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
	delete mDataCache;
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

void OmSegmentation::Mesh()
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

void OmSegmentation::RebuildChunk(const OmMipChunkCoord & mipCoord, const OmSegIDsSet & rModifiedValues)
{
	assert(0);

	//build chunk volume data and analyze data
//	OmMipVolume::BuildChunk(mipCoord);

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
		return mGroups->UnsetGroup(set, name);
		(new OmSegmentGroupAction(GetId(), set, name, false))->Run();
	} else {
		assert(0 && "only unset regular groups");
	}
}


void OmSegmentation::DeleteGroup(OmGroupID)
{
	printf("FIXME delete group not supported\n");
}

void OmSegmentation::FlushDirtySegments()
{
	mSegmentCache->flushDirtySegments();
}

void OmSegmentation::FlushDend()
{
	mst_->FlushDend(this);
}

void OmSegmentation::FlushDendUserEdges()
{
	mst_->FlushDendUserEdges(this);
}

void OmSegmentation::SetDendThreshold( float t )
{
	if( t == mst_->mDendThreshold ){
		return;
	}
	mst_->mDendThreshold = t;
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

float OmSegmentation::GetDendThreshold()
{
	return mst_->mDendThreshold;
}

boost::shared_ptr<OmMST> OmSegmentation::getMST()
{
	return mst_;
}

OmDataWrapperPtr OmSegmentation::doExportChunk(const OmMipChunkCoord& coord)
{
	OmMipChunkPtr chunk;
	getDataCache()->Get(chunk, coord, true);

	OmImage<uint32_t, 3> imageData = chunk->GetCopyOfChunkDataAsOmImage32();
	boost::shared_ptr<uint32_t> rawDataPtr = imageData.getMallocCopyOfData();
	uint32_t* rawData = rawDataPtr.get();

	for(int i = 0; i < chunk->GetNumberOfVoxelsInChunk(); ++i){
		if( 0 != rawData[i]) {
			rawData[i] = mSegmentCache->findRootID(rawData[i]);
		}
	}

	return OmDataWrapperFactory::produce(rawDataPtr);
}


class OmSegmentationChunkBuildTask : public zi::Runnable {
private:
	const OmMipChunkCoord coord_;
	OmMipVolume* vol_;
	boost::shared_ptr<OmSegmentCache> segmentCache_;

public:
	OmSegmentationChunkBuildTask(const OmMipChunkCoord& coord,
								 boost::shared_ptr<OmSegmentCache> segmentCache,
								 OmMipVolume* vol)
		:coord_(coord), vol_(vol), segmentCache_(segmentCache)
	{}

	void run()
	{
		OmMipChunkPtr chunk;
		vol_->GetChunk(chunk, coord_);

		const bool isMIPzero = chunk->IsLeaf();

		chunk->RefreshDirectDataValues(isMIPzero, segmentCache_);
	}
};

void OmSegmentation::doBuildThreadedVolume()
{
	OmThreadPool threadPool;
	threadPool.start(30);

	for (int level = 0; level <= GetRootMipLevel(); ++level) {
		const Vector3i dims = MipLevelDimensionsInMipChunks(level);
		for (int z = 0; z < dims.z; ++z){
			for (int y = 0; y < dims.y; ++y){
				for (int x = 0; x < dims.x; ++x){

					OmMipChunkCoord coord(level, x, y, z);

					boost::shared_ptr<OmSegmentationChunkBuildTask> task =
						boost::make_shared<OmSegmentationChunkBuildTask>(coord,
																		 mSegmentCache,
																		 this);
					threadPool.addTaskBack(task);
				}
			}
		}
	}

	threadPool.join();
}
