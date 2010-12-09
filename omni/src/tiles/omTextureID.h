#ifndef OM_TEXTURE_ID_H
#define OM_TEXTURE_ID_H

/*
 * OmTextureIDs are a wrapper for GLuint texture IDs.
 *
 * They associate a texture ID with the corresponding TileCoord,
 * and keep track of the size of the texture in memory.
 *
 * A TextureID is associated with a TextureIDCache that keeps track of how
 * much GL video memory is in use.
 *
 * Rachel Shearer - rshearer@mit.edu - 3/17/09
 */

#include "common/omGl.h"

#include <boost/variant.hpp>

class OmTextureID {
public:
	OmTextureID();
	OmTextureID(const Vector2i&, boost::shared_ptr<uint8_t>);
	OmTextureID(const Vector2i&, boost::shared_ptr<OmColorRGBA>);
	virtual ~OmTextureID();

	int GetWidth() const {
		return dims_.x;
	}

	int GetHeight() const {
		return dims_.y;
	}

	GLuint GetTextureID() const
	{
		if(!textureID_){
			throw OmIoException("texture not yet built");
		}
		return *textureID_;
	}

	uint64_t NumBytes() const {
		return numBytes_;
	}

	void* GetTileData() const;

	bool NeedToBuildTexture() const
	{
		return (flag_ == OMTILE_NEEDTEXTUREBUILT ||
				flag_ == OMTILE_NEEDCOLORMAP);
	}

	GLint GetGLformat() const
	{
		switch(flag_){
		case OMTILE_NEEDCOLORMAP:
			return GL_RGBA;
		case OMTILE_NEEDTEXTUREBUILT:
			return GL_LUMINANCE;
		default:
			throw OmArgException("unknown flag");
		}
	}

	void TextureBindComplete(const GLuint textureID)
	{
		flag_ = OMTILE_GOOD;
		textureID_ = boost::optional<GLuint>(textureID);
		deleteTileData();
	}

private:
	enum OmTileFlag {
		OMTILE_COORDINVALID = 0,
		OMTILE_NEEDCOLORMAP,
		OMTILE_NEEDTEXTUREBUILT,
		OMTILE_GOOD
	};

	boost::optional<GLuint> textureID_;
	const Vector2i dims_;
	OmTileFlag flag_;
	const uint64_t numBytes_;

	void deleteTileData();

	// free data once texture is built
	boost::variant<boost::shared_ptr<uint8_t>,
				   boost::shared_ptr<OmColorRGBA> > tileData_;

};

#endif
