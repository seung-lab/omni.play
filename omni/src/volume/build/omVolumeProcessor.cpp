#include "volume/build/omVolumeProcessor.h"
#include "threads/omTaskManager.hpp"
#include "chunks/omChunkCoord.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "segment/omSegments.h"
#include "chunks/omChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"

class OmSegmentationChunkBuildTask : public zi::runnable {
private:
    const OmChunkCoord coord_;
    OmSegmentation *const vol_;
    OmSegments *const segments_;

public:
    OmSegmentationChunkBuildTask(const OmChunkCoord& coord,
                                 OmSegments* segments,
                                 OmSegmentation* vol)
        : coord_(coord)
        , vol_(vol)
        , segments_(segments)
    {}

    void run()
    {
        OmSegChunk* chunk = vol_->GetChunk(coord_);

        const bool isMIPzero = (0 == coord_.Level);

        chunk->SegData()->ProcessChunk(isMIPzero, segments_);

        const ChunkUniqueValues segIDs =
            vol_->ChunkUniqueValues()->RereadChunk(coord_, 1);

        // std::cout << "chunk " << coord_
        //           << " has " << segIDs.size()
        //           << " values\n";

        if(isMIPzero){
            // vol_->updateMinMax(chunk->GetMinValue(),
            //                    chunk->GetMaxValue());
        }
    }
};

void OmVolumeProcessor::doBuildThreadedVolume(OmSegmentation* vol)
{
    OmThreadPool threadPool;
    threadPool.start();

    om::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
        vol->GetMipChunkCoords();

    FOR_EACH(iter, *coordsPtr)
    {
        const OmChunkCoord& coord = *iter;

        om::shared_ptr<OmSegmentationChunkBuildTask> task =
            om::make_shared<OmSegmentationChunkBuildTask>(coord,
                                                          vol->Segments(),
                                                          vol);
        threadPool.push_back(task);
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
        // OmChunk* chunk = vol_->GetChunk(coord_);

        // const bool isMIPzero = (0 == coord_.Level);

        // if(isMIPzero){
        //     vol_->updateMinMax(chunk->GetMinValue(),
        //                        chunk->GetMaxValue());
        // }
    }
};

void OmVolumeProcessor::doBuildThreadedVolume(OmChannel* vol)
{
    OmThreadPool threadPool;
    threadPool.start();

    om::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
        vol->GetMipChunkCoords();

    FOR_EACH(iter, *coordsPtr){
        const OmChunkCoord& coord = *iter;

        om::shared_ptr<OmChannelImplChunkBuildTask> task =
            om::make_shared<OmChannelImplChunkBuildTask>(coord, vol);
        threadPool.push_back(task);
    }

    threadPool.join();
    //printf("max is %g\n", mMaxVal);
}
