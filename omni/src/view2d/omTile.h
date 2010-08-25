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
	
	void* GetImageData(const OmTileCoord &key, Vector2<int> &sliceDims, OmMipVolume *vol);
	OmMipChunkCoord TileToMipCoord(const OmTileCoord &key);
	int GetDepth(const OmTileCoord &key);

private:
	OmViewGroupState * mViewGroupState;

	ViewType view_type;
	ObjectType vol_type;
	OmId myID;
	
	float mAlpha;
	int mSamplesPerVoxel;
	int mBytesPerSample;
	
	int mBackgroundSamplesPerVoxel;
	int mBackgroundBytesPerSample;

	unsigned char * setMyColorMap(OmSegID* imageData, Vector2<int> dims, const OmTileCoord &key);

	OmTextureIDPtr doBindToTextureID(const OmTileCoord & key, OmTileCache * cache);

};

#endif
