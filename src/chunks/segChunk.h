#pragma once

/**
 * segChunk extends chunk with extra funtionality specific to
 *   manipulating segmentation data.
 *
 *  Michael Purcaro - purcaro@gmail.com - 1/29/11
 */

#include "chunks/chunk.h"
#include "zi/omMutex.h"

class segmentation;
namespace om { namespace segchunk { class dataInterface; } }

class segChunk : public chunk {
private:
    segmentation *const vol_;
    const boost::scoped_ptr<om::segchunk::dataInterface> segChunkData_;

    std::set<segId> modifiedSegIDs_;
    zi::spinlock modifiedSegIDsLock_;

public:
    segChunk(segmentation* vol, const coords::chunkCoord& coord);
    virtual ~segChunk();

    uint32_t GetVoxelValue(const coords::dataCoord& vox);
    void SetVoxelValue(const coords::dataCoord& vox, const uint32_t value);

    om::segchunk::dataInterface* SegData(){
        return segChunkData_.get();
    }
};

