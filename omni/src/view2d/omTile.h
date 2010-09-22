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
	OmTile(ViewType, ObjectType, OmId, OmMipVolume*, OmViewGroupState*);
	~OmTile();

	OmTextureIDPtr BindToTextureID(const OmTileCoord &, OmTileCache*);
	void SetNewAlpha(const float);
	OmMipChunkCoord TileToMipCoord(const OmTileCoord &);

	OmMipVolume *const mVolume;
	OmMipVolume* getVol(){ return mVolume; }

private:
	const Vector2i dims_;
	OmViewGroupState * mViewGroupState;

	const ViewType view_type;
	const ObjectType vol_type;
	const OmId myID;

	float mAlpha;

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
