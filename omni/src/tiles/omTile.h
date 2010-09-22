#ifndef OM_TILE_H
#define OM_TILE_H

/*
 *	OmTile allows access to 2D image data from the source volume.
 *
 *	Rachel Shearer - rshearer@mit.edu
 */

#include "common/omStd.h"
#include "volume/omMipChunkCoord.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTileTypes.hpp"

#include <QColor>

class OmMipVolume;
class OmCacheBase;
class OmViewGroupState;

class OmTile {
public:
	OmTile(OmCacheBase* cache, const OmTileCoord& key);
	~OmTile(){}

	OmTextureIDPtr loadData();
	const OmTextureIDPtr& getTexture(){ return texture_; }
	const OmTileCoord& getTileCoord(){ return key_; }

	void Flush(){} // for cache

private:
	OmCacheBase *const cache_;
	const OmTileCoord key_;
	const int tileLength_;
	const Vector2i dims_;
	const OmMipChunkCoord mipChunkCoord_;

	OmTextureIDPtr texture_;

	boost::shared_ptr<uint8_t> GetImageData8bit();
	boost::shared_ptr<uint32_t> GetImageData32bit();

	void makeNullTextureID();
	OmMipChunkCoord TileToMipCoord();
	void doLoadData();
	bool isMipChunkCoordValid();
	int GetDepth();
	int getVolDepth();
	void setVertices(const int x, const int y, const float zoomFactor);

	OmMipVolume* getVol(){ return key_.getVolume(); }
	ObjectType getVolType();
};

#endif
