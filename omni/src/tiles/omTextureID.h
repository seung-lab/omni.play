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
#include "tiles/omTile.h"

#include "boost/variant.hpp"

class OmCacheBase;

class OmTextureID {
public:
	OmTextureID();
	OmTextureID(const Vector2i&, OmCacheBase*);
	virtual ~OmTextureID();

	void setData(boost::shared_ptr<uint8_t>);
	void setData(boost::shared_ptr<OmColorRGBA>);

	int getWidth() const { return dims_.x; }
	int getHeight() const { return dims_.y; }
	GLuint getTextureID() const { return textureID_; }

	int getNumBytes() const { return mem_size; }
	void* getTileData() const;

	bool needToBuildTexture() const {
		return (flag_ == OMTILE_NEEDTEXTUREBUILT ||
			flag_ == OMTILE_NEEDCOLORMAP);
	}

	GLint getGLformat() const {
		switch(flag_){
		case OMTILE_NEEDCOLORMAP:
			return GL_RGBA;
		case OMTILE_NEEDTEXTUREBUILT:
			return GL_LUMINANCE;
		default:
			throw OmArgException("unknown flag");
		}
	}

	void textureBindComplete(const GLuint textureID){
		flag_ = OMTILE_GOOD;
		textureID_ = textureID;
		deleteTileData();
	}

private:
	enum OmTileFlag {
		OMTILE_COORDINVALID = 0,
		OMTILE_NEEDCOLORMAP,
		OMTILE_NEEDTEXTUREBUILT,
		OMTILE_GOOD
	};

	OmCacheBase *const cache_;
	GLuint textureID_;
	const Vector2i dims_;

	// Free this once the texture is built.
	boost::variant<boost::shared_ptr<uint8_t>,
		       boost::shared_ptr<OmColorRGBA> > tileData_;

        OmTileFlag flag_;

	int mem_size;	// total size of data in memory: width * height
	                //  * bytesPerSample * samplesPerVoxel

	void deleteTileData();
};

#endif
