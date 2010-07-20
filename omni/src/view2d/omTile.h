#ifndef OM_TILE_H
#define OM_TILE_H

/*
 *	OmTile allows access to 2D image data from the source volume.
 *
 *	Rachel Shearer - rshearer@mit.edu
 */

#include "common/omStd.h"
#include "volume/omMipChunkCoord.h"
#include "system/omCacheBase.h"
#include "volume/omSegmentation.h"

#include <QExplicitlySharedDataPointer>
#include <QColor>

class OmMipVolume;
class OmTextureID;
class OmTileCoord;
class OmThreadedCachingTile;
class OmViewGroupState;

class OmTile {

public:
	OmTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol, OmViewGroupState * vgs);
	~OmTile();
	
	OmTextureID* BindToTextureID(const OmTileCoord &key, OmThreadedCachingTile* cache);
	
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

	void setMyColorMap(OmSegID* imageData, Vector2<int> dims, const OmTileCoord &key, void **rData);

	OmTextureID * doBindToTextureID(const OmTileCoord & key, OmThreadedCachingTile* cache);

};

#endif
