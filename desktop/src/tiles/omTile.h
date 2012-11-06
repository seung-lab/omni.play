#pragma once

#include "common/omStd.h"
#include "tiles/omTileCoord.h"

class OmTextureID;
class OmMipVolume;
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
    const om::coords::Chunk mipChunkCoord_;

    boost::scoped_ptr<OmTextureID> texture_;

    void load8bitChannelTile();
    void load32bitSegmentationTile();

    om::coords::Chunk tileToMipCoord();
    int getDepth();
    void setVertices(const int x, const int y, const float zoomFactor);

    inline OmMipVolume* getVol() const {
        return key_.getVolume();
    }

    om::common::ObjectType getVolType() const;
};

