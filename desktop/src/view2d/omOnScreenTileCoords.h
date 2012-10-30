#pragma once

#include "common/common.h"
#include "tiles/omTileImplTypes.hpp"

class OmView2dState;
class OmFilter2d;

class OmOnScreenTileCoords {
public:
    OmOnScreenTileCoords(OmView2dState*, OmMipVolume*);

    OmTileCoordsAndLocationsPtr ComputeCoordsAndLocations();
    OmTileCoordsAndLocationsPtr ComputeCoordsAndLocations(const int);

private:
    OmView2dState *const state_;
    OmMipVolume *const vol_;
    const ViewType viewType_;
    OmViewGroupState *const vgs_;

    int mipLevel_;
    OmTileCoordsAndLocationsPtr tileCoordsAndLocations_;

    uint64_t freshness_;

    void doComputeCoordsAndLocations(const int depthOffset);
    GLfloatBox computeVertices(const om::coords::Chunk& coord, const OmMipVolume* vol);
    void computeTile(const om::coords::Chunk&, const int);
    void makeTileCoordFromFilter(OmFilter2d*, const om::coords::Chunk &, const int);
    OmTileCoord makeTileCoord(const om::coords::Chunk&, const int, OmMipVolume*, int);

    friend std::ostream& operator<<(std::ostream &out, const OmOnScreenTileCoords& c);
};

