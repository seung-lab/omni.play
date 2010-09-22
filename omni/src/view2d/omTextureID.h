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
enum OmTileFlag {
	OMTILE_COORDINVALID = 0,
	OMTILE_NEEDCOLORMAP,
	OMTILE_NEEDTEXTUREBUILT,
	OMTILE_GOOD
};

class OmTileCache;

class OmTextureID : public OmCacheableBase {

public:
	OmTextureID();
	OmTextureID(const Vector2i&, OmTileCache*);
	virtual ~OmTextureID();

	void setData(boost::shared_ptr<uint8_t>);
	void setData(boost::shared_ptr<OmColorRGBA>);

	bool needToBuildTexture(){
		return (flag_ == OMTILE_NEEDTEXTUREBUILT ||
			flag_ == OMTILE_NEEDCOLORMAP);
	}

	int getWidth() const { return dims_.x; }
	int getHeight() const { return dims_.y; }

	GLuint getTextureID() const { return textureID; }
	void setTextureID(const GLuint id) { textureID = id; }

	OmTileFlag getFlag() const { return flag_; }
	void setFlag(const OmTileFlag f){ flag_ = f; }

	void Flush(){}

	void* getTileData();
	void deleteTileData();

private:
	GLuint textureID;
	const Vector2i dims_;

	// Free this once the texture is built.
	boost::variant<boost::shared_ptr<uint8_t>,
		       boost::shared_ptr<OmColorRGBA> > tileData_;

        OmTileFlag flag_;

	int mem_size;	// total size of data in memory: width * height * bytesPerSample * samplesPerVoxel
};

#endif
