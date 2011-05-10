#pragma once

/**
 * OmSegChunk extends OmChunk with extra funtionality specific to
 *   manipulating segmentation data.
 *
 *  Michael Purcaro - purcaro@gmail.com - 1/29/11
 */

#include "chunks/omChunk.h"
#include "chunks/omSegChunkData.h"
#include "zi/omMutex.h"

class OmSegmentation;

class OmSegChunk : public OmChunk {
private:
    OmSegmentation *const vol_;
    const boost::scoped_ptr<OmSegChunkData> segChunkData_;

    std::set<OmSegID> modifiedSegIDs_;
    zi::spinlock modifiedSegIDsLock_;

public:
    OmSegChunk(OmSegmentation* vol, const OmChunkCoord& coord);
    virtual ~OmSegChunk();

    uint32_t GetVoxelValue(const DataCoord& vox);
    void SetVoxelValue(const DataCoord& vox, const uint32_t value);

    OmSegChunkData* SegData(){
        return segChunkData_.get();
    }
};

