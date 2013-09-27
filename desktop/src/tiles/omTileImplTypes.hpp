#pragma once

#include "utility/glInclude.h"
#include "tiles/omTileCoord.h"
#include "utility/omSharedPtr.hpp"
#include "tiles/omTileTypes.hpp"

#include <deque>

struct GLfloatPair {
    GLfloat x;
    GLfloat y;
};

struct GLfloatBox {
    GLfloatPair lowerLeft;
    GLfloatPair lowerRight;
    GLfloatPair upperRight;
    GLfloatPair upperLeft;
};

std::ostream& operator<<(std::ostream &out, const GLfloatPair& b);
std::ostream& operator<<(std::ostream &out, const GLfloatBox& b);

struct OmTileCoordAndVertices {
    OmTileCoord tileCoord;
    GLfloatBox vertices;
};

struct TextureVectices {
    GLfloatPair upperLeft;
    GLfloatPair lowerRight;
};

struct OmTileAndVertices {
    OmTilePtr tile;
    GLfloatBox vertices;
    TextureVectices textureVectices;
};

typedef std::deque<OmTileCoordAndVertices> OmTileCoordsAndLocations;
typedef std::shared_ptr<OmTileCoordsAndLocations> OmTileCoordsAndLocationsPtr;

std::ostream& operator<<(std::ostream &out, const TextureVectices& v);

