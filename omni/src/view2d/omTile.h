#ifndef OM_TILE_H
#define OM_TILE_H

/*
 *	OmTile allows access to 2D image data from the source volume.
 *
 *	Rachel Shearer - rshearer@mit.edu
 */

#include "common/omStd.h"
#include "volume/omMipChunkCoord.h"
#include "system/cache/omCacheBase.h"
#include "volume/omSegmentation.h"

#include <QColor>

class OmMipVolume;
class OmTextureID;
class OmTileCoord;
class OmTileCache;
class OmViewGroupState;

typedef boost::shared_ptr<OmTextureID> OmTextureIDPtr;

class OmTile {

public:
	OmTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol, OmViewGroupState * vgs);
	~OmTile();

	OmTextureIDPtr BindToTextureID(const OmTileCoord &key, OmTileCache * cache);
	void SetNewAlpha(float newval);
	OmMipVolume *mVolume;
	OmMipChunkCoord TileToMipCoord(const OmTileCoord &key);

private:
	const Vector2i dims_;
	OmViewGroupState * mViewGroupState;

	ViewType view_type;
	ObjectType vol_type;
	OmId myID;

	float mAlpha;
	int mSamplesPerVoxel;
	int mBytesPerSample;

	int mBackgroundSamplesPerVoxel;
	int mBackgroundBytesPerSample;

	OmTextureIDPtr doBindToTextureID(const OmTileCoord & key, OmTileCache * cache);

	boost::shared_ptr<OmColorRGBA> setMyColorMap(boost::shared_ptr<uint32_t>,
						     const Vector2i&,
						     const OmTileCoord&);

	boost::shared_ptr<uint8_t> GetImageData8bit(const OmTileCoord&);
	boost::shared_ptr<uint32_t> GetImageData32bit(const OmTileCoord&);

	OmTextureIDPtr makeNullTextureID();
	int GetDepth(const OmTileCoord &key);
	int getVolDepth(const OmTileCoord& key);
};

#endif
