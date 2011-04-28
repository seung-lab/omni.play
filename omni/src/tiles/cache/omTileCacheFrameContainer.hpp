#ifndef OM_TILE_CACHE_FRAME_CONTAINER_HPP
#define OM_TILE_CACHE_FRAME_CONTAINER_HPP


class OmTileCacheFrameContainer {
private:
    OmMipVolume *const vol_;

public:
    OmTileCacheFrameContainer(OmMipVolume* vol)
        : vol_(vol)
    {}

};

#endif
