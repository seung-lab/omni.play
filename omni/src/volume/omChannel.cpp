#include "common/omCommon.h"
#include "common/omDebug.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "project/omProject.h"
#include "project/omProjectSaveAction.h"
#include "system/cache/omMipVolumeCache.h"
#include "utility/OmThreadPool.hpp"
#include "volume/build/omVolumeImporter.hpp"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"
#include "volume/omMipChunk.h"
#include "volume/omVolumeData.hpp"

#include <float.h>

#include "zi/omThreads.h"

OmChannel::OmChannel()
	: mDataCache(new OmMipVolumeCache(this))
	, mVolData(new OmVolumeData())
{
	mWasBounded = false;
}

OmChannel::OmChannel(OmId id)
	: OmManageableObject(id)
	, mDataCache(new OmMipVolumeCache(this))
	, mVolData(new OmVolumeData())
{
	mWasBounded = false;

	//do not use meta data
	mStoreChunkMetaData = false;

	//build blank data
	BuildVolumeData();

	AddFilter();
}

OmChannel::~OmChannel()
{
	delete mDataCache;
}

boost::shared_ptr<OmVolumeData> OmChannel::getVolData() {
	return mVolData;
}

std::string OmChannel::GetName(){
	return "channel" +  boost::lexical_cast<std::string>(GetId());
}

std::string OmChannel::GetDirectoryPath() {
	return OmDataPaths::getDirectoryPath(this);
}

void OmChannel::BuildVolumeData()
{
	OmDataPath path = OmDataPath(OmDataPaths::getDefaultHDF5channelDatasetName());
	OmMipVolume::Build(path);
}

OmFilter2d& OmChannel::AddFilter() {
	OmFilter2d& filter = mFilter2dManager.AddFilter();
	(new OmProjectSaveAction())->Run();
	return filter;
}

OmFilter2d& OmChannel::GetFilter(OmId id) {
	return mFilter2dManager.GetFilter(id);
}

const OmIDsSet & OmChannel::GetValidFilterIds(){
	return mFilter2dManager.GetValidFilterIds();
}

bool OmChannel::IsFilterValid(const OmId id){
	return mFilter2dManager.IsFilterValid(id);
}

bool OmChannel::IsFilterEnabled(OmId id){
	return mFilter2dManager.IsFilterEnabled(id);
}

void OmChannel::CloseDownThreads(){
	mDataCache->closeDownThreads();
}

bool OmChannel::ImportSourceData(OmDataPath & dataset)
{
	OmVolumeImporter<OmChannel> importer(this);
	return importer.Import(dataset);
}

void OmChannel::loadVolData(){
	mVolData->load(this);
}

OmDataWrapperPtr OmChannel::doExportChunk(const OmMipChunkCoord& coord)
{
	OmMipChunkPtr chunk;
	getDataCache()->Get(chunk, coord, true);

	OmImage<uint32_t, 3> imageData = chunk->GetCopyOfChunkDataAsOmImage32();
	boost::shared_ptr<uint32_t> rawDataPtr = imageData.getMallocCopyOfData();
	return OmDataWrapperFactory::produce(rawDataPtr);
}


class OmChannelChunkBuildTask : public zi::runnable {
private:
	const OmMipChunkCoord coord_;
	OmMipVolume* vol_;

public:
	OmChannelChunkBuildTask(const OmMipChunkCoord& coord,
							OmMipVolume* vol)
		:coord_(coord), vol_(vol)
	{}

	void run()
	{
		OmMipChunkPtr chunk;
		vol_->GetChunk(chunk, coord_);

		const bool isMIPzero = chunk->IsLeaf();

		if(isMIPzero) {
			printf("channelfixme");
			//vol_->mMaxVal = std::max(p_chunk->getMax(), mMaxVal);
			//vol_->mMinVal = std::min(p_chunk->getMin(), mMinVal);
		}
	}
};

void OmChannel::doBuildThreadedVolume()
{
	OmThreadPool threadPool;
	threadPool.start(5);

	for (int level = 0; level <= GetRootMipLevel(); ++level) {
		const Vector3i dims = MipLevelDimensionsInMipChunks(level);
		for (int z = 0; z < dims.z; ++z){
			for (int y = 0; y < dims.y; ++y){
				for (int x = 0; x < dims.x; ++x){

					OmMipChunkCoord coord(level, x, y, z);

					boost::shared_ptr<OmChannelChunkBuildTask> task =
						boost::make_shared<OmChannelChunkBuildTask>(coord, this);
					threadPool.addTaskBack(task);
				}
			}
		}
	}

	threadPool.join();
	mWasBounded = false;
}
