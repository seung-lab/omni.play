#include "volume/build/omVolumeProcessor.h"
#include "utility/omThreadPool.hpp"
#include "chunks/omChunkCoord.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentCache.h"
#include "chunks/omChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"

class OmSegmentationChunkBuildTask : public zi::runnable {
private:
	const OmChunkCoord coord_;
	OmSegmentation *const vol_;
	OmSegmentCache *const segmentCache_;

public:
	OmSegmentationChunkBuildTask(const OmChunkCoord& coord,
								 OmSegmentCache* segmentCache,
								 OmSegmentation* vol)
		: coord_(coord)
		, vol_(vol)
		, segmentCache_(segmentCache)
	{}

	void run()
	{
		OmSegChunkPtr chunk;
		vol_->GetChunk(chunk, coord_);

		const bool isMIPzero = (0 == coord_.Level);

		chunk->SegData()->RefreshDirectDataValues(isMIPzero, segmentCache_);

		const ChunkUniqueValues segIDs =
			vol_->ChunkUniqueValues()->Values(coord_, 1);

		std::cout << "chunk " << coord_
				  << " has " << segIDs.size()
				  << " values\n";

		if(isMIPzero){
			//vol_->updateMinMax(chunk->GetMinValue(),
			//				   chunk->GetMaxValue());
		}
	}
};

void OmVolumeProcessor::doBuildThreadedVolume(OmSegmentation* vol)
{
	OmThreadPool threadPool;
	threadPool.start();

	boost::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
		vol->GetMipChunkCoords();

	FOR_EACH(iter, *coordsPtr){
		const OmChunkCoord& coord = *iter;

		boost::shared_ptr<OmSegmentationChunkBuildTask> task =
			boost::make_shared<OmSegmentationChunkBuildTask>(coord,
															 vol->SegmentCache(),
															 vol);
		threadPool.addTaskBack(task);
	}

	threadPool.join();
	//printf("max is %g\n", mMaxVal);
}


class OmChannelImplChunkBuildTask : public zi::runnable {
private:
	const OmChunkCoord coord_;
	OmChannelImpl* vol_;

public:
	OmChannelImplChunkBuildTask(const OmChunkCoord& coord,
								OmChannelImpl* vol)
		:coord_(coord), vol_(vol)
	{}

	void run()
	{
		OmChunkPtr chunk;
		vol_->GetChunk(chunk, coord_);

		const bool isMIPzero = (0 == coord_.Level);

		if(isMIPzero){
			//vol_->updateMinMax(chunk->GetMinValue(),
			//				   chunk->GetMaxValue());
		}
	}
};

void OmVolumeProcessor::doBuildThreadedVolume(OmChannel* vol)
{
	OmThreadPool threadPool;
	threadPool.start();

	boost::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
		vol->GetMipChunkCoords();

	FOR_EACH(iter, *coordsPtr){
		const OmChunkCoord& coord = *iter;

		boost::shared_ptr<OmChannelImplChunkBuildTask> task =
			boost::make_shared<OmChannelImplChunkBuildTask>(coord, vol);
		threadPool.addTaskBack(task);
	}

	threadPool.join();
	//printf("max is %g\n", mMaxVal);
}
