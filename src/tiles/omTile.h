#pragma once

/*
 *  OmTile allows access to 2D image data from the source volume.
 *
 *  Rachel Shearer - rshearer@mit.edu
 */

#include "common/omStd.h"
#include "tiles/omTileCoord.h"

class OmTextureID;
class mipVolume;
class OmCacheBase;
class OmViewGroupState;

class OmTile {
public:
    OmTile(OmCacheBase* cache, const OmTileCoord& key);
    ~OmTile();

    void LoadData();
    uint32_t NumBytes() const;

    inline OmTextureID& GetTexture() {
        return *texture_;
    }

    inline const OmTileCoord& GetTileCoord() const {
        return key_;
    }

private:
    OmCacheBase *const cache_;
    const OmTileCoord key_;
    const int tileLength_;
    const om::chunkCoord mipChunkCoord_;

    boost::scoped_ptr<OmTextureID> texture_;

    void load8bitChannelTile();
    void load32bitSegmentationTile();

    om::chunkCoord tileToMipCoord();
    int getDepth();
    int getChunkSliceNum();
    void setVertices(const int x, const int y, const float zoomFactor);

    inline mipVolume* getVol() const {
        return key_.getVolume();
    }

    ObjectType getVolType() const;
};

