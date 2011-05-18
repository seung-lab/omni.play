#include "chunks/omChunkCoord.h"
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

void OmUpdateBoundingBoxes::doUpdate(const OmChunkCoord& coord)
{
    OmSegChunk* chunk = vol_->GetChunk(coord);

    chunk->SegData()->RefreshBoundingData(segments_);
}

void OmUpdateBoundingBoxes::Update()
{
    OmThreadPool pool;
    pool.start();

    om::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
        vol_->GetMipChunkCoords(0);

    FOR_EACH(iter, *coordsPtr)
    {
        pool.push_back(
            zi::run_fn(
                zi::bind(&OmUpdateBoundingBoxes::doUpdate,
                         this, *iter)));
    }

    pool.join();
}
