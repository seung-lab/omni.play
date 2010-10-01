#include "tiles/omTile.h"
#include "view2d/omView2dState.hpp"
#include "volume/omMipVolume.h"
#include "view2d/omOnScreenTileCoords.h"
#include "system/cache/omCacheManager.h"
#include "system/viewGroup/omViewGroupState.h"

#include <boost/make_shared.hpp>

OmOnScreenTileCoords::OmOnScreenTileCoords(const boost::shared_ptr<OmView2dState>& state)
	: state_(state)
	, mVolume(state->getVol())
	, viewType_(state->getViewType())
	, mViewGroupState(state->getViewGroupState())
	, tileLength_(state->getVol()->GetChunkDimension())
	, zoomFactor_(state->getZoomScale())
	, mipLevel_(state->getMipLevel())
	, stretch_(state->getVol()->GetStretchValues(state->getViewType()))
	, totalViewport_(state->getTotalViewport())
	, translateVector_(state->ComputePanDistance())
	, tileCoordsAndLocations_(boost::make_shared<OmTileCoordsAndLocations>())
{
	freshness_ = 0;
	if(SEGMENTATION == mVolume->getVolumeType()){
		freshness_ = OmCacheManager::GetFreshness();
	}

	setDepths();
}

void OmOnScreenTileCoords::setDepths()
{
	Vector3f depth(0,0,0);
	state_->setViewTypeDepth(depth, state_->getSliceDepth());
	const DataCoord data_coord = mVolume->SpaceToDataCoord(depth);
	dataDepth_ = state_->getViewTypeDepth(data_coord);
}

OmTileCoordsAndLocationsPtr
OmOnScreenTileCoords::ComputeCoordsAndLocations()
{
	return ComputeCoordsAndLocations(0);
}

OmTileCoordsAndLocationsPtr
OmOnScreenTileCoords::ComputeCoordsAndLocations(const int depthOffset)
{
	const float levelFactor = om::pow2(mipLevel_);
	const int dataDim = tileLength_ * levelFactor;

	if(translateVector_.y < 0) {
		yMipChunk_ = ((abs((int)translateVector_.y) /dataDim)) * dataDim * levelFactor;
		yval = (-1 * (abs((int)translateVector_.y) % dataDim));
	} else {
		yMipChunk_ = 0;
		yval = translateVector_.y;
	}

	const float maxY = totalViewport_.height/zoomFactor_/stretch_.y;
	for(float y = yval; y < maxY;
	    y += tileLength_, yMipChunk_ += dataDim){

		if (translateVector_.x < 0) {
			xMipChunk_ = ((abs((float)translateVector_.x) / dataDim)) * dataDim * levelFactor;
			xval = (-1 * (abs((float)translateVector_.x) % dataDim));
		} else {
			xMipChunk_ = 0;
			xval = translateVector_.x;
		}

		const float maxX = totalViewport_.width/zoomFactor_/stretch_.x;
		for(float x = xval; x < maxX;
		    x += tileLength_, xMipChunk_ += dataDim){

			computeTile(x, y, depthOffset);
		}
	}

	return tileCoordsAndLocations_;
}

void OmOnScreenTileCoords::computeTile(const float x, const float y,
				       const int depthOffset)
{
	const DataCoord dataCoord = toDataCoord(depthOffset);;
	const OmMipChunkCoord chunkCoord =
		mVolume->DataToMipCoord(dataCoord, mipLevel_);

	if(!mVolume->ContainsMipChunkCoord(chunkCoord)){
		return;
	}

	const SpaceCoord spaceCoord = mVolume->DataToSpaceCoord(dataCoord);

	OmTileCoordAndVertices pair = {makeTileCoord(spaceCoord),
				       computeVertices(x, y) };

	tileCoordsAndLocations_->push_back(pair);
}

DataCoord OmOnScreenTileCoords::toDataCoord(const int depthOffset)
{
	//TODO: yMipChunk_ and yMipChunk_ are floats; convert to int?
	return state_->makeViewTypeVector3<int>(xMipChunk_,
						yMipChunk_,
						dataDepth_ + depthOffset);
}

OmTileCoord OmOnScreenTileCoords::makeTileCoord(const SpaceCoord& coord)
{
	return OmTileCoord(mipLevel_,
			   coord,
			   mVolume,
			   freshness_,
			   mViewGroupState,
			   viewType_);
}

GLfloatBox OmOnScreenTileCoords::computeVertices(const float xRaw,
						 const float yRaw)
{
	const float x = xRaw * stretch_.x;
	const float y = yRaw * stretch_.y;

	const GLfloat minX = x * zoomFactor_;
	const GLfloat minY = y * zoomFactor_;
	const GLfloat maxX = (x + tileLength_*stretch_.x) * zoomFactor_;
	const GLfloat maxY = (y + tileLength_*stretch_.y) * zoomFactor_;

	GLfloatBox glBox;
	glBox.lowerLeft.y  = minY;
	glBox.upperRight.y = maxY;

	switch(viewType_){
	case XY_VIEW:
	case XZ_VIEW:
		glBox.lowerLeft.x  = minX;
		glBox.upperRight.x = maxX;
		break;
	case YZ_VIEW:
		glBox.lowerLeft.x  = maxX;
		glBox.upperRight.x = minX;
		break;
	}

	glBox.lowerRight.x = glBox.upperRight.x;
	glBox.lowerRight.y = glBox.lowerLeft.y;

	glBox.upperLeft.x  = glBox.lowerLeft.x;
	glBox.upperLeft.y  = glBox.upperRight.y;

	return glBox;
}
