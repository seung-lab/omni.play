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
#include "utility/image/omImage.hpp"

#include <QColor>

class OmMipVolume;
class OmCacheBase;
class OmViewGroupState;

class OmTile {
public:
	OmTile(OmCacheBase* cache, const OmTileCoord& key);
	~OmTile(){}

	void LoadData();
	uint32_t NumBytes() const;
	void Flush(){}

	const OmTextureIDPtr& GetTexture(){ return texture_; }
	const OmTileCoord& GetTileCoord(){ return key_; }

private:
	OmCacheBase *const cache_;
	const OmTileCoord key_;
	const int tileLength_;
	const Vector2i dims_;
	const OmMipChunkCoord mipChunkCoord_;

	OmTextureIDPtr texture_;

	OmImage<uint8_t, 2> getImageData8bit();
	boost::shared_ptr<uint32_t> getImageData32bit();

	void makeNullTextureID();
	OmMipChunkCoord tileToMipCoord();
	void doLoadData();
	bool isMipChunkCoordValid();
	int getDepth();
	int getVolDepth();
	void setVertices(const int x, const int y, const float zoomFactor);

	OmMipVolume* getVol(){ return key_.getVolume(); }
	ObjectType getVolType();
};

#endif
