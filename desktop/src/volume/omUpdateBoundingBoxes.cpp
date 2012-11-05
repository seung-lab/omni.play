#include "chunks/omSegChunk.h"
#include "chunks/omSegChunkDataInterface.hpp"
#include "segment/omSegments.h"
#include "threads/omTaskManager.hpp"
#include "volume/omSegmentation.h"
#include "volume/omUpdateBoundingBoxes.h"

OmUpdateBoundingBoxes::OmUpdateBoundingBoxes(OmSegmentation* vol)
    : vol_(vol)
    , segments_(vol->Segments())
{}

void OmUpdateBoundingBoxes::doUpdate(const om::coords::Chunk& coord)
{
    OmSegChunk* chunk = vol_->GetChunk(coord);

    chunk->SegData()->RefreshBoundingData(segments_);
}

void OmUpdateBoundingBoxes::Update()
{
    OmThreadPool pool;
    pool.start();

    boost::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
        vol_->MipChunkCoords(0);

    FOR_EACH(iter, *coordsPtr)
    {
        pool.push_back(
            zi::run_fn(
                zi::bind(&OmUpdateBoundingBoxes::doUpdate,
                         this, *iter)));
    }

    pool.join();
}
