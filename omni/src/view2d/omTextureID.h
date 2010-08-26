#ifndef OM_TEXTURE_ID_H
#define OM_TEXTURE_ID_H

/*
 *      omTextureIDs are a wrapper for GLuint texture IDs.  They associate a texture ID with
 *      the corresponding TileCoord, and keep track of the size of the texture in memory.
 *
 *	A TextureID is associated with a TextureIDCache that keeps track of how much GL video memory is in use.
 *
 *	Rachel Shearer - rshearer@mit.edu - 3/17/09
 */

#include "common/omStd.h"
#include "common/omGl.h"
#include "system/cache/omCacheableBase.h"
#include "system/omGenericManager.h"
#include "omTileCoord.h"
#include "omTile.h"

#include "boost/variant.hpp"

// Flags to OmTextureID. FIXME. All of this is just a hack to make it work for now. MW.
#define OMTILE_FIXME		0
#define OMTILE_NEEDTEXTUREBUILT 1
#define OMTILE_COORDINVALID     2
#define OMTILE_GOOD		3
#define OMTILE_NEEDCOLORMAP 	4

class OmTileCache;

class OmTextureID : public OmCacheableBase {

public:
	OmTextureID(const OmTileCoord & tileCoord,
		    const Vector2i& dims,
		    OmTileCache* cache,
		    boost::shared_ptr<uint8_t> texture,
		    int flags);

	OmTextureID(const OmTileCoord & tileCoord,
		    const Vector2i& dims,
		    OmTileCache* cache,
		    boost::shared_ptr<uint32_t> texture,
		    int flags);

	virtual ~OmTextureID();

	int getX() const { return dims_.x; }
	int getY() const { return dims_.y; }

	GLuint getTextureID() const { return textureID; }
	void setTextureID(const GLuint id) { textureID = id; }

	const OmTileCoord& GetCoordinate() const { return mTileCoordinate; }

	int getFlags() const { return flags; }
	void setFlags(const int f){ flags = f; }

	void Flush(){}

	void* getTexture();
	void deleteTexture();

private:
	const OmTileCoord mTileCoordinate;
	GLuint textureID;
	const Vector2i dims_;
	boost::variant<boost::shared_ptr<uint8_t>,
		       boost::shared_ptr<uint32_t> > texture_;
        int flags;

	int mem_size;	// total size of data in memory: width * height * bytesPerSample * samplesPerVoxel
};

#endif
