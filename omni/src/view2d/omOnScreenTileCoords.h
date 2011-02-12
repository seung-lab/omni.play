#ifndef OM_ON_SCREEN_TILE_COORDS_H
#define OM_ON_SCREEN_TILE_COORDS_H

#include "common/omCommon.h"
#include "tiles/omTileTypes.hpp"

class OmView2dState;
class OmFilter2d;

class OmOnScreenTileCoords {
public:
	OmOnScreenTileCoords(const boost::shared_ptr<OmView2dState>&);

	OmTileCoordsAndLocationsPtr ComputeCoordsAndLocations();
	OmTileCoordsAndLocationsPtr ComputeCoordsAndLocations(const int);

private:
	const boost::shared_ptr<OmView2dState> state_;
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
	void makeTileCoordFromFilter(OmFilter2d& filter, const SpaceCoord & spaceCoord, const float x, const float y);
	OmTileCoord makeTileCoord(const SpaceCoord& coord, OmMipVolume * vol, int freshness);

};

#endif
