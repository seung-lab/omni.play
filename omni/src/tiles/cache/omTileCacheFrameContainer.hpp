#pragma once


class OmTileCacheFrameContainer {
private:
    OmMipVolume *const vol_;

public:
    OmTileCacheFrameContainer(OmMipVolume* vol)
        : vol_(vol)
    {}

};

