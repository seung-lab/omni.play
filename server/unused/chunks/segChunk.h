#pragma once

/**
 * segChunk extends chunk with extra funtionality specific to
 *   manipulating segmentation data.
 *
 *  Michael Purcaro - purcaro@gmail.com - 1/29/11
 */

#include "chunks/chunk.h"
#include "zi/mutex.h"

namespace om {
namespace volume { class segmentation; }
namespace segchunk { class dataInterface; }

namespace chunks {
class segChunk : public chunk {
private:
    volume::segmentation *const vol_;
    const boost::scoped_ptr<segchunk::dataInterface> segChunkData_;

    std::set<common::segId> modifiedSegIDs_;
    zi::spinlock modifiedSegIDsLock_;

public:
    segChunk(volume::segmentation* vol, const coords::chunkCoord& coord);
    virtual ~segChunk();

    uint32_t GetVoxelValue(const coords::dataCoord& vox);
    void SetVoxelValue(const coords::dataCoord& vox, const uint32_t value);

    segchunk::dataInterface* SegData(){
        return segChunkData_.get();
    }
};

}
}
