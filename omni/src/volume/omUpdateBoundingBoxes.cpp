#include "utility/omThreadPool.hpp"
#include "volume/omUpdateBoundingBoxes.h"
#include "chunks/omSegChunk.h"
#include "chunks/omChunkCoord.h"
#include "segment/omSegmentCache.h"
#include "volume/omSegmentation.h"

class UpdateBoundingBox : public zi::runnable {
private:
	OmSegmentation *const vol_;
	OmSegmentCache *const segmentCache_;
	const OmChunkCoord coord_;

public:
	UpdateBoundingBox(const OmChunkCoord& coord,
					  OmSegmentCache* segmentCache,
					  OmSegmentation* vol)
		: vol_(vol)
		, segmentCache_(segmentCache)
		, coord_(coord)
	{}

	void run()
	{
		OmSegChunkPtr chunk;
		vol_->GetChunk(chunk, coord_);

		chunk->SegData()->RefreshBoundingData(segmentCache_);
	}
};

void OmUpdateBoundingBoxes::Update()
{
	OmThreadPool threadPool;
	threadPool.start();

	boost::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
		vol_->GetMipChunkCoords(0);

	FOR_EACH(iter, *coordsPtr){
		const OmChunkCoord& coord = *iter;

		boost::shared_ptr<UpdateBoundingBox> task =
			boost::make_shared<UpdateBoundingBox>(coord,
												  vol_->SegmentCache(),
												  vol_);
		threadPool.addTaskBack(task);
	}

	threadPool.join();
}
