#pragma once

/**
 * OmSegChunk extends OmChunk with extra funtionality specific to
 *   manipulating segmentation data.
 *
 *  Michael Purcaro - purcaro@gmail.com - 1/29/11
 */

#include "chunks/omChunk.h"
#include "zi/omMutex.h"

class segmentation;
namespace om { namespace segchunk { class dataInterface; } }

class OmSegChunk : public OmChunk {
private:
    segmentation *const vol_;
    const boost::scoped_ptr<om::segchunk::dataInterface> segChunkData_;

    std::set<OmSegID> modifiedSegIDs_;
    zi::spinlock modifiedSegIDsLock_;

public:
    OmSegChunk(segmentation* vol, const om::chunkCoord& coord);
    virtual ~OmSegChunk();

    uint32_t GetVoxelValue(const om::dataCoord& vox);
    void SetVoxelValue(const om::dataCoord& vox, const uint32_t value);

    om::segchunk::dataInterface* SegData(){
        return segChunkData_.get();
    }
};

