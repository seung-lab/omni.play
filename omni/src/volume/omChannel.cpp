#include "common/omCommon.h"
#include "common/omDebug.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "project/omProject.h"
#include "actions/omActions.hpp"
#include "system/cache/omMipVolumeCache.h"
#include "utility/omThreadPool.hpp"
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

OmChannel::OmChannel(OmID id)
	: OmManageableObject(id)
	, mDataCache(new OmMipVolumeCache(this))
	, mVolData(new OmVolumeData())
{
	mWasBounded = false;

	//do not use meta data
	mStoreChunkMetaData = false;

	mFilter2dManager.AddFilter();
}

OmChannel::~OmChannel()
{
	delete mDataCache;
}

boost::shared_ptr<OmVolumeData> OmChannel::getVolData() {
	return mVolData;
}

std::string OmChannel::GetName(){
	return "channel" + om::NumToStr(GetID());
}

std::string OmChannel::GetDirectoryPath() {
	return OmDataPaths::getDirectoryPath(this);
}

OmFilter2d& OmChannel::AddFilter() {
	OmFilter2d& filter = mFilter2dManager.AddFilter();
	OmActions::Save();
	return filter;
}

OmFilter2d& OmChannel::GetFilter(OmID id) {
	return mFilter2dManager.GetFilter(id);
}

const OmIDsSet & OmChannel::GetValidFilterIds(){
	return mFilter2dManager.GetValidFilterIds();
}

bool OmChannel::IsFilterValid(const OmID id){
	return mFilter2dManager.IsFilterValid(id);
}

bool OmChannel::IsFilterEnabled(OmID id){
	return mFilter2dManager.IsFilterEnabled(id);
}

void OmChannel::CloseDownThreads(){
}

void OmChannel::loadVolData()
{
	if(IsVolumeReadyForDisplay()){
		mVolData->load(this);
	}
}

OmDataWrapperPtr OmChannel::doExportChunk(const OmMipChunkCoord& coord,
										  const bool)
{
	OmMipChunkPtr chunk;
	mDataCache->Get(chunk, coord);

	OmImage<uint32_t, 3> imageData = chunk->GetCopyOfChunkDataAsOmImage32();
	boost::shared_ptr<uint32_t> rawDataPtr = imageData.getMallocCopyOfData();
	return OmDataWrapperFactory::produce(rawDataPtr);
}


class OmChannelChunkBuildTask : public zi::runnable {
private:
	const OmMipChunkCoord coord_;
	OmChannel* vol_;

public:
	OmChannelChunkBuildTask(const OmMipChunkCoord& coord,
							OmChannel* vol)
		:coord_(coord), vol_(vol)
	{}

	void run()
	{
		OmMipChunkPtr chunk;
		vol_->GetChunk(chunk, coord_);

		const bool isMIPzero = (0 == coord_.Level);

		if(isMIPzero){
			vol_->updateMinMax(chunk->GetMinValue(),
							   chunk->GetMaxValue());
		}
	}
};

void OmChannel::doBuildThreadedVolume()
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

					boost::shared_ptr<OmChannelChunkBuildTask> task =
						boost::make_shared<OmChannelChunkBuildTask>(coord, this);
					threadPool.addTaskBack(task);
				}
			}
		}
	}

	threadPool.join();
	printf("max is %g\n", mMaxVal);
	mWasBounded = true;
}

int OmChannel::GetBytesPerSample() const
{
	return mVolData->GetBytesPerSample();
}

void OmChannel::SetVolDataType(const OmVolDataType type)
{
	mVolDataType = type;
	getVolData()->setDataType(this);
}
