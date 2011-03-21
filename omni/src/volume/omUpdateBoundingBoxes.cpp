#include "utility/omThreadPool.hpp"
#include "volume/omUpdateBoundingBoxes.h"
#include "chunks/omSegChunk.h"
#include "chunks/omChunkCoord.h"
#include "segment/omSegments.h"
#include "volume/omSegmentation.h"

OmUpdateBoundingBoxes::OmUpdateBoundingBoxes(OmSegmentation* vol)
    : vol_(vol)
    , segments_(vol->Segments())
{}

void OmUpdateBoundingBoxes::doUpdate(const OmChunkCoord& coord)
{
    OmSegChunkPtr chunk;
    vol_->GetChunk(chunk, coord);

    chunk->SegData()->RefreshBoundingData(segments_);
}

void OmUpdateBoundingBoxes::Update()
{
    OmThreadPool pool;
    pool.start();

    boost::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
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
