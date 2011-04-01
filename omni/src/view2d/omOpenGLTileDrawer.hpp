#ifndef OM_OPENGL_TILE_DRAWER_HPP
#define OM_OPENGL_TILE_DRAWER_HPP

#include "common/om.hpp"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTileTypes.hpp"
#include "zi/omUtility.h"

class OmOpenGLTileDrawer {
private:
    const ViewType viewType_;

public:
    OmOpenGLTileDrawer(const ViewType viewType)
        : viewType_(viewType)
    {}

    ~OmOpenGLTileDrawer()
    {}

    void DrawTiles(const std::deque<OmTileAndVertices>& tilesToDraw)
    {
        FOR_EACH(it, tilesToDraw){
            drawTile(*it);
        }
    }

private:
    void drawTile(const OmTileAndVertices& tv)
    {
        if(viewType_ == YZ_VIEW) {
            glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
            glTranslatef(0.5, 0.5, 0.0);
            glRotatef(-90, 0.0, 0.0, 1.0);
            glTranslatef(-0.5, -0.5, 0.0);
            glMatrixMode(GL_MODELVIEW);
        }

        const OmTextureIDPtr& texture = tv.tile->GetTexture();
        if(texture->NeedToBuildTexture()){
            doBindTileDataToGLid(texture);
        }

        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());

        const TextureVectices& textureVectices = tv.textureVectices;

        glBegin(GL_QUADS);
        glTexCoord2f(textureVectices.upperLeft.x,
                     textureVectices.lowerRight.y);  /* lower left corner */
        glVertex2f(tv.vertices.lowerLeft.x,
                   tv.vertices.lowerLeft.y);

        glTexCoord2f(textureVectices.lowerRight.x,
                     textureVectices.lowerRight.y);  /* lower right corner */
        glVertex2f(tv.vertices.lowerRight.x,
                   tv.vertices.lowerRight.y);

        glTexCoord2f(textureVectices.lowerRight.x,
                     textureVectices.upperLeft.y);  /* upper right corner */
        glVertex2f(tv.vertices.upperRight.x,
                   tv.vertices.upperRight.y);

        glTexCoord2f(textureVectices.upperLeft.x,
                     textureVectices.upperLeft.y);  /* upper left corner */
        glVertex2f(tv.vertices.upperLeft.x,
                   tv.vertices.upperLeft.y);
        glEnd();
    }

    void doBindTileDataToGLid(const OmTextureIDPtr& texture)
    {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glTexImage2D(GL_TEXTURE_2D,
                     0, // mipmap 0
                     texture->GetGLinternalFormat(),
                     texture->GetWidth(),
                     texture->GetHeight(),
                     0, // no border
                     texture->GetGLdataFormat(),
                     texture->GetPixelDataType(),
                     texture->GetTileData()
            );

        texture->TextureBindComplete(textureID);
    }
};

#endif
