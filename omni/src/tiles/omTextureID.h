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
#include "common/omCommon.h"

#include <QImage>

class OmPooledTileWrapper;
template <class> class OmPooledTile;

class OmTextureID {
public:
    OmTextureID();
    OmTextureID(const Vector2i&, OmPooledTile<uint8_t>*);
    OmTextureID(const Vector2i&, OmPooledTile<OmColorARGB>*);

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

    void* GetTileData() const;

    uchar* GetTileDataUChar() const;

    bool NeedToBuildTexture() const
    {
        return (flag_ == OMTILE_NEEDTEXTUREBUILT ||
                flag_ == OMTILE_NEEDCOLORMAP);
    }

    QImage::Format GetQTimageFormat() const
    {
        switch(flag_){
        case OMTILE_NEEDCOLORMAP:
            return QImage::Format_ARGB32_Premultiplied;
        case OMTILE_NEEDTEXTUREBUILT:
            return QImage::Format_Indexed8;
        default:
            throw OmArgException("unknown flag");
        }
    }

    GLint GetGLinternalFormat() const
    {
        switch(flag_){
        case OMTILE_NEEDCOLORMAP:
            return GL_RGBA8;
        case OMTILE_NEEDTEXTUREBUILT:
            return GL_LUMINANCE;
        default:
            throw OmArgException("unknown flag");
        }
    }

    GLenum GetGLdataFormat() const
    {
        switch(flag_){
        case OMTILE_NEEDCOLORMAP:
            return GL_BGRA;
        case OMTILE_NEEDTEXTUREBUILT:
            return GL_LUMINANCE;
        default:
            throw OmArgException("unknown flag");
        }
    }

    GLenum GetPixelDataType() const
    {
        switch(flag_){
        case OMTILE_NEEDCOLORMAP:
            return GL_UNSIGNED_INT_8_8_8_8;
        case OMTILE_NEEDTEXTUREBUILT:
            return GL_UNSIGNED_BYTE;
        default:
            throw OmArgException("unknown flag");
        }
    }

    void TextureBindComplete(const GLuint textureID)
    {
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

    boost::optional<GLuint> textureID_;
    const Vector2i dims_;
    OmTileFlag flag_;

    OmPooledTileWrapper* pooledTile_;

    void deleteTileData();
};

#endif
