#pragma once

#include "common/omCommon.h"
#include "tiles/omTileImplTypes.hpp"

class OmView2dState;
class OmFilter2d;

class OmOnScreenTileCoords {
public:
    OmOnScreenTileCoords(OmView2dState*);

    OmTileCoordsAndLocationsPtr ComputeCoordsAndLocations();
    OmTileCoordsAndLocationsPtr ComputeCoordsAndLocations(const int);

private:
    OmView2dState *const state_;
    OmMipVolume *const vol_;
    const ViewType viewType_;
    OmViewGroupState *const vgs_;
    const float tileLength_;
    const float zoomFactor_;
    const float mipLevel_;
    const Vector2f stretch_;
    const Vector4i totalViewport_; //lower left x, lower left y, width, height
    const Vector2f translateVector_;

    int dataDepth_;
    uint64_t freshness_;

    int xMipChunk_;
    int yMipChunk_;

    OmTileCoordsAndLocationsPtr tileCoordsAndLocations_;

    void doComputeCoordsAndLocations(const int depthOffset);
    void setDepths();
    DataCoord toDataCoord(const int);
    GLfloatBox computeVertices(const float, const float);
    void computeTile(const float x, const float y, const int);
    void makeTileCoordFromFilter(OmFilter2d* filter, const DataCoord & dataCoord,
                                 const float x, const float y);
    OmTileCoord makeTileCoord(const DataCoord& coord, OmMipVolume * vol,
                              int freshness);

    friend std::ostream& operator<<(std::ostream &out, const OmOnScreenTileCoords& c);
};

