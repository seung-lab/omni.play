#ifndef OM_ON_SCREEN_TILE_COORDS_H
#define OM_ON_SCREEN_TILE_COORDS_H

#include "common/omCommon.h"
#include "tiles/omTileTypes.hpp"

class OmView2dState;

class OmOnScreenTileCoords {
public:
	OmOnScreenTileCoords(const boost::shared_ptr<OmView2dState>&);

	OmTileCoordsAndLocationsPtr ComputeCoordsAndLocations();
	OmTileCoordsAndLocationsPtr ComputeCoordsAndLocations(const int);

private:
	const boost::shared_ptr<OmView2dState> state_;
	OmMipVolume *const mVolume;
	const ViewType viewType_;
	OmViewGroupState *const mViewGroupState;
	const float tileLength_;
	const float zoomFactor_;
	const float mipLevel_;
	const Vector2f stretch_;
	const Vector4i totalViewport_; //lower left x, lower left y, width, height
	const Vector2f translateVector_;

	int dataDepth_;
	uint64_t freshness_;

	float xMipChunk_;
	float yMipChunk_;
	float xval;
	float yval;

	OmTileCoordsAndLocationsPtr tileCoordsAndLocations_;

	void setDepths();
        DataCoord toDataCoord(const int);
	OmTileCoord makeTileCoord(const SpaceCoord&);
	GLfloatBox computeVertices(const float, const float);
	void computeTile(const float x, const float y, const int);
};

#endif
