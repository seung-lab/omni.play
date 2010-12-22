#include "tiles/omTile.h"
#include "view2d/omView2dState.hpp"
#include "volume/omMipVolume.h"
#include "volume/omChannel.h"
#include "project/omProject.h"
#include "volume/omFilter2d.h"
#include "view2d/omOnScreenTileCoords.h"
#include "system/cache/omCacheManager.h"
#include "viewGroup/omViewGroupState.h"

#include <boost/make_shared.hpp>

OmOnScreenTileCoords::OmOnScreenTileCoords(const boost::shared_ptr<OmView2dState>& state)
	: state_(state)
	, vol_(state->getVol())
	, viewType_(state->getViewType())
	, vgs_(state->getViewGroupState())
	, tileLength_(vol_->GetChunkDimension())
	, zoomFactor_(state->getZoomScale())
	, mipLevel_(state->getMipLevel())
	, stretch_(vol_->GetStretchValues(state->getViewType()))
	, totalViewport_(state->getTotalViewport())
	, translateVector_(state->ComputePanDistance())
	, tileCoordsAndLocations_(boost::make_shared<OmTileCoordsAndLocations>())
{
	freshness_ = 0;
	if(SEGMENTATION == vol_->getVolumeType()){
		freshness_ = OmCacheManager::GetFreshness();
	}

	setDepths();
}

void OmOnScreenTileCoords::setDepths()
{
	Vector3f depth(0,0,0);
	state_->setViewTypeDepth(depth, state_->getSliceDepth());
	const DataCoord data_coord = vol_->SpaceToDataCoord(depth);
	//printf("depth: %f,%f,%f data: %i,%i,%i\n", DEBUGV3(depth), DEBUGV3(data_coord));
	dataDepth_ = state_->getViewTypeDepth(depth);

	Vector3f res = vol_->GetDataResolution();
	switch(viewType_){
	case XY_VIEW:
		dataDepth_ /= res.z;
		break;
	case XZ_VIEW:
		dataDepth_ /= res.y;
		break;
	case YZ_VIEW:
		dataDepth_ /= res.x;
		break;
	}
}

OmTileCoordsAndLocationsPtr
OmOnScreenTileCoords::ComputeCoordsAndLocations()
{
	return ComputeCoordsAndLocations(0);
}

OmTileCoordsAndLocationsPtr
OmOnScreenTileCoords::ComputeCoordsAndLocations(const int depthOffset)
{
	if(vol_->IsVolumeReadyForDisplay()){
		doComputeCoordsAndLocations(depthOffset);
	}

	OmTileCoordsAndLocationsPtr ret = tileCoordsAndLocations_;
	tileCoordsAndLocations_ = boost::make_shared<OmTileCoordsAndLocations>();
	return ret;
}

void OmOnScreenTileCoords::doComputeCoordsAndLocations(const int depthOffset)
{
	const int levelFactor = om::pow2int(mipLevel_);
	const int dataDim = tileLength_ * levelFactor;
	float xval;
	float yval;

	if(translateVector_.y < 0) {
		yMipChunk_ = ((abs((int)translateVector_.y) /dataDim)) * dataDim * levelFactor;
		yval = (-1 * (abs((int)translateVector_.y) % dataDim));
	} else {
		yMipChunk_ = 0;
		yval = translateVector_.y;
	}


	const float maxY = totalViewport_.height/zoomFactor_/stretch_.y;
	for(float y = yval; y < maxY; y += tileLength_, yMipChunk_ += dataDim){

		if (translateVector_.x < 0) {
			xMipChunk_ = ((abs((int)translateVector_.x) / dataDim)) * dataDim * levelFactor;
			xval = (-1 * (abs((int)translateVector_.x) % dataDim));
		} else {
			xMipChunk_ = 0;
			xval = translateVector_.x;
		}

		const float maxX = totalViewport_.width/zoomFactor_/stretch_.x;
		for(float x = xval; x < maxX; x += tileLength_, xMipChunk_ += dataDim){

			computeTile(x, y, depthOffset);
		}
	}
}

void OmOnScreenTileCoords::computeTile(const float x, const float y,
									   const int depthOffset)
{
	const DataCoord dataCoord = toDataCoord(depthOffset);;
	const OmMipChunkCoord chunkCoord =
		vol_->DataToMipCoord(dataCoord, mipLevel_);

	if(!vol_->ContainsMipChunkCoord(chunkCoord)){
		return;
	}

	const SpaceCoord spaceCoord = vol_->DataToSpaceCoord(dataCoord);

	if(depthOffset){ // i.e. if we are pre-fetching
		if(CHANNEL == vol_->getVolumeType()) {
			OmChannel& chan = OmProject::GetChannel(vol_->getID());
			foreach( OmID id, chan.GetValidFilterIds() ) {
				OmFilter2d &filter = chan.GetFilter(id);
				makeTileCoordFromFilter(filter, spaceCoord, x, y);
			}
		}
	}

	OmTileCoordAndVertices pair = {makeTileCoord(spaceCoord, vol_, freshness_),
								   computeVertices(x, y) };

	tileCoordsAndLocations_->push_back(pair);
}

void OmOnScreenTileCoords::makeTileCoordFromFilter(OmFilter2d& filter,
												   const SpaceCoord & spaceCoord,
												   const float x, const float y)
{
	if(!filter.HasValidVol()){
		return;
	}

	OmTileCoordAndVertices pair =
		{ makeTileCoord(spaceCoord,
						filter.GetMipVolume(),
						OmCacheManager::GetFreshness()),
		  computeVertices(x, y) };

	tileCoordsAndLocations_->push_back(pair);
}

DataCoord OmOnScreenTileCoords::toDataCoord(const int depthOffset)
{
	Vector3f res = vol_->GetDataResolution();
	int off;
	switch(viewType_){
	case XY_VIEW:
		off = depthOffset / res.z;
		break;
	case XZ_VIEW:
		off = depthOffset / res.y;
		break;
	case YZ_VIEW:
		off = depthOffset / res.x;
		break;
	}

	//printf("dataDepth_: %i depthOffset: %i\n", dataDepth_, depthOffset);
	//TODO: yMipChunk_ and yMipChunk_ are floats; convert to int?
	return state_->makeViewTypeVector3<int>(xMipChunk_,
											yMipChunk_,
											dataDepth_ + off);
}

OmTileCoord OmOnScreenTileCoords::makeTileCoord(const SpaceCoord& coord,
												OmMipVolume* vol,
												int freshness)
{

	return OmTileCoord(mipLevel_,
					   coord,
					   vol,
					   freshness,
					   vgs_,
					   viewType_,
					   state_->getObjectType());
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
