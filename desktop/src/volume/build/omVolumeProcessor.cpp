#include "volume/build/omVolumeProcessor.h"
#include "threads/omTaskManager.hpp"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omAffinityChannel.h"
#include "segment/omSegments.h"
#include "chunks/omChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"

class OmSegmentationChunkBuildTask : public zi::runnable {
private:
    const om::chunkCoord coord_;
    OmSegmentation *const vol_;
    OmSegments *const segments_;

public:
    OmSegmentationChunkBuildTask(const om::chunkCoord& coord,
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

    std::shared_ptr<std::deque<om::chunkCoord> > coordsPtr =
        vol->GetMipChunkCoords();

    FOR_EACH(iter, *coordsPtr)
    {
        const om::chunkCoord& coord = *iter;

        std::shared_ptr<OmSegmentationChunkBuildTask> task =
            std::make_shared<OmSegmentationChunkBuildTask>(coord,
                                                          vol->Segments(),
                                                          vol);
        threadPool.push_back(task);
    }

    threadPool.join();
    //printf("max is %g\n", mMaxVal);
}

void OmVolumeProcessor::doBuildThreadedVolume(OmChannel*)
{

}

void OmVolumeProcessor::doBuildThreadedVolume(OmAffinityChannel*)
{
    
}
