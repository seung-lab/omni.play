#include "mesh/omMipMeshManager.h"
#include "common/omCommon.h"
#include "common/omDebug.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omIDataReader.h"
#include "segment/io/omMST.h"
#include "mesh/omMipMesh.h"
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
#include "system/cache/omCacheManager.h"
#include "system/omGenericManager.h"
#include "system/omGroups.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "utility/omThreadPool.hpp"
#include "utility/omTimer.h"
#include "volume/build/omVolumeImporter.hpp"
#include "volume/omMipChunk.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "volume/omVolumeData.hpp"
#include "zi/omThreads.h"

#include <QFile>
#include <QTextStream>

// used by OmDataArchiveProject
OmSegmentation::OmSegmentation()
	: mDataCache(new OmMipVolumeCache(this))
	, mVolData(new OmVolumeData())
	, mSegmentCache(new OmSegmentCache(this))
	, mSegmentLists(new OmSegmentLists())
	, mGroups(boost::make_shared<OmGroups>(this))
	, mst_(boost::make_shared<OmMST>(this))
	, mMipMeshManager(boost::make_shared<OmMipMeshManager>(this))
{}

// used by OmGenericManager
OmSegmentation::OmSegmentation(OmID id)
	: OmManageableObject(id)
	, mDataCache(new OmMipVolumeCache(this))
	, mVolData(new OmVolumeData())
	, mSegmentCache(new OmSegmentCache(this))
	, mSegmentLists(new OmSegmentLists())
	, mGroups(boost::make_shared<OmGroups>(this))
	, mst_(boost::make_shared<OmMST>(this))
	, mMipMeshManager(boost::make_shared<OmMipMeshManager>(this))
{
	//uses meta data
	mStoreChunkMetaData = true;

	mSegmentCache->refreshTree();
}

OmSegmentation::~OmSegmentation()
{
	delete mSegmentCache;
	delete mDataCache;
}

boost::shared_ptr<OmVolumeData> OmSegmentation::getVolData() {
	return mVolData;
}

std::string OmSegmentation::GetName(){
	return "segmentation" +  boost::lexical_cast<std::string>(GetID());
}

std::string OmSegmentation::GetDirectoryPath() {
	return OmDataPaths::getDirectoryPath(this);
}

/////////////////////////////////
///////          Build Methods

void OmSegmentation::BuildVolumeData()
{
	OmDataPath dataset = OmDataPath("main");
	OmMipVolume::Build(dataset);
}

void OmSegmentation::Mesh()
{
	ziMesher mesher(GetID(), mMipMeshManager.get(), GetRootMipLevel());
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

void OmSegmentation::MeshChunk(const OmMipChunkCoord& coord)
{
	ziMesher mesher(GetID(), mMipMeshManager.get(), GetRootMipLevel());
	mesher.addChunkCoord(coord);
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
		  MeshingManager* meshingMan = new MeshingManager( GetID(), &mMipMeshManager );
		  meshingMan->addToQueue( mipCoord );
		  meshingMan->start();
		  meshingMan->wait();
		*/
	}

	//remove mesh from cache to force it to reload
	foreach( const OmSegID & val, rModifiedValues ){
		OmMipMeshCoord mip_mesh_coord = OmMipMeshCoord(mipCoord, val);
		mMipMeshManager->UncacheMesh(mip_mesh_coord);
	}

	//call redraw to force mesh to reload
	OmEvents::Redraw3d();
}

boost::shared_ptr<std::set<OmSegment*> >
OmSegmentation::GetAllChildrenSegments(const OmSegIDsSet& set)
{
	OmSegmentIterator iter(mSegmentCache);
	iter.iterOverSegmentIDs(set);

	OmSegment * seg = iter.getNextSegment();
	std::set<OmSegment*>* children = new std::set<OmSegment*>();

	while(NULL != seg) {
		children->insert(seg);
		seg = iter.getNextSegment();
	}

	return boost::shared_ptr<std::set<OmSegment*> >(children);
}

void OmSegmentation::SetDendThreshold(const double t)
{
	if( t == mst_->UserThreshold() ){
		return;
	}
	mst_->SetUserThreshold(t);
	mSegmentCache->refreshTree();
}

void OmSegmentation::CloseDownThreads()
{
	mMipMeshManager->CloseDownThreads();
}

DataCoord OmSegmentation::FindCenterOfSelectedSegments() const
{
	DataBbox box;

	OmSegmentIterator iter(mSegmentCache);
	iter.iterOverSelectedIDs();

	uint32_t counter = 0;

	OmSegment* seg = iter.getNextSegment();
	while(NULL != seg) {
		const DataBbox& segBox = seg->getBounds();
		if(segBox.isEmpty()){
			continue;
		}

		box.merge(segBox);

		++counter;
		if(counter > 5000) {
			break;
		}

		seg = iter.getNextSegment();
	}

	if(!counter){
		return Vector3i(0,0,0);
	}

	return (box.getMin() + box.getMax()) / 2;
}

bool OmSegmentation::ImportSourceData(const OmDataPath& path)
{
	OmVolumeImporter<OmSegmentation> importer(this, path);
	return importer.Import();
}

void OmSegmentation::loadVolData()
{
	if(IsVolumeReadyForDisplay()){
		mVolData->load(this);
	}
}

double OmSegmentation::GetDendThreshold()
{
	return mst_->UserThreshold();
}

OmDataWrapperPtr OmSegmentation::doExportChunk(const OmMipChunkCoord& coord,
											   const bool rerootSegments)
{
	std::cout << "\r\texporting " << coord << std::flush;

	OmMipChunkPtr chunk;
	mDataCache->Get(chunk, coord);

	OmImage<uint32_t, 3> imageData = chunk->GetCopyOfChunkDataAsOmImage32();
	boost::shared_ptr<uint32_t> rawDataPtr = imageData.getMallocCopyOfData();
	uint32_t* rawData = rawDataPtr.get();

	for(int i = 0; i < chunk->GetNumberOfVoxelsInChunk(); ++i){
		if(rerootSegments){
			if( 0 != rawData[i]) {
				rawData[i] = mSegmentCache->findRootID(rawData[i]);
			}
		}
	}

	return OmDataWrapperFactory::produce(rawDataPtr);
}


class OmSegmentationChunkBuildTask : public zi::runnable {
private:
	const OmMipChunkCoord coord_;
	OmSegmentation* vol_;
	OmSegmentCache* segmentCache_;

public:
	OmSegmentationChunkBuildTask(const OmMipChunkCoord& coord,
								 OmSegmentCache* segmentCache,
								 OmSegmentation* vol)
		:coord_(coord), vol_(vol), segmentCache_(segmentCache)
	{}

	void run()
	{
		OmMipChunkPtr chunk;
		vol_->GetChunk(chunk, coord_);

		const bool isMIPzero = (0 == coord_.Level);

		chunk->RefreshDirectDataValues(isMIPzero, segmentCache_);
		std::cout << "chunk " << coord_
				  << " has " << chunk->GetDirectDataValues().size()
				  << " values\n";

		if(isMIPzero){
			vol_->updateMinMax(chunk->GetMinValue(),
							   chunk->GetMaxValue());
		}
	}
};

void OmSegmentation::doBuildThreadedVolume()
{
	loadVolData();

	OmThreadPool threadPool;
	threadPool.start();

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
	printf("max is %g\n", mMaxVal);
	mWasBounded = true;
}

void OmSegmentation::GetMesh(OmMipMeshPtr& ptr,
							 const OmMipChunkCoord& coord,
							 const OmSegID segID )
{
	return mMipMeshManager->GetMesh(ptr, OmMipMeshCoord(coord, segID));
}

int OmSegmentation::GetBytesPerSample() const
{
	return mVolData->GetBytesPerSample();
}

void OmSegmentation::SetVolDataType(const OmVolDataType type)
{
	mVolDataType = type;
	getVolData()->setDataType(this);
}

SegmentationDataWrapper OmSegmentation::getSDW() const
{
	return SegmentationDataWrapper(GetID());
}

void OmSegmentation::Flush()
{
	mSegmentCache->Flush();
	mst_->Flush();
}
