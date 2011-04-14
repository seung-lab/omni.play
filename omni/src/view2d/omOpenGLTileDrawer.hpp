#ifndef OM_OPENGL_TILE_DRAWER_HPP
#define OM_OPENGL_TILE_DRAWER_HPP

#include "common/om.hpp"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTileTypes.hpp"
#include "zi/omUtility.h"

class OmOpenGLTileDrawer {
public:
    void DrawTiles(std::deque<OmTileAndVertices>& tilesToDraw)
    {
        FOR_EACH(iter, tilesToDraw)
        {
            drawTile(iter->tile->GetTexture(),
                     iter->vertices,
                     iter->textureVectices);
        }
    }

private:
    void drawTile(OmTextureID& texture,
                  const GLfloatBox& vertices,
                  const TextureVectices& textureVectices)
    {
        if(texture.NeedToBuildTexture()){
            doBindTileDataToGLid(texture);
        }

        glBindTexture(GL_TEXTURE_2D, texture.GetTextureID());

        // std::cout << "drawing: " << tv.tile->GetTileCoord() << "\n";

        glBegin(GL_QUADS);
        glTexCoord2f(textureVectices.upperLeft.x,
                     textureVectices.lowerRight.y);  /* lower left corner */
        glVertex2f(vertices.lowerLeft.x,
                   vertices.lowerLeft.y);
        // std::cout << "\ttex: " << textureVectices.upperLeft.x
        //           << ", " << textureVectices.lowerRight.y << "\n";
        // std::cout << "\tvertex: " << vertices.lowerLeft.x
        //           << ", " << vertices.lowerLeft.y << "\n";

        glTexCoord2f(textureVectices.lowerRight.x,
                     textureVectices.lowerRight.y);  /* lower right corner */
        glVertex2f(vertices.lowerRight.x,
                   vertices.lowerRight.y);
        // std::cout << "\ttex: " << textureVectices.lowerRight.x
        //           << ", " << textureVectices.lowerRight.y << "\n";
        // std::cout << "\tvertex: " << vertices.lowerRight.x
        //           << ", " << vertices.lowerRight.y << "\n";

        glTexCoord2f(textureVectices.lowerRight.x,
                     textureVectices.upperLeft.y);  /* upper right corner */
        glVertex2f(vertices.upperRight.x,
                   vertices.upperRight.y);
        // std::cout << "\ttex: " << textureVectices.lowerRight.x
        //           << ", " << textureVectices.upperLeft.y << "\n";
        // std::cout << "\tvertex: " << vertices.upperRight.x
        //           << ", " << vertices.upperRight.y << "\n";

        glTexCoord2f(textureVectices.upperLeft.x,
                     textureVectices.upperLeft.y);  /* upper left corner */
        glVertex2f(vertices.upperLeft.x,
                   vertices.upperLeft.y);
        // std::cout << "\ttex: " << textureVectices.upperLeft.x
        //           << ", " << textureVectices.upperLeft.y << "\n";
        // std::cout << "\tvertex: " << vertices.upperLeft.x
        //           << ", " << vertices.upperLeft.y << "\n";

        glEnd();
    }

    void doBindTileDataToGLid(OmTextureID& texture)
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
                     texture.GetGLinternalFormat(),
                     texture.GetWidth(),
                     texture.GetHeight(),
                     0, // no border
                     texture.GetGLdataFormat(),
                     texture.GetPixelDataType(),
                     texture.GetTileData()
            );

        texture.TextureBindComplete(textureID);
    }
};

#endif
