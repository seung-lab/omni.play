#ifndef OM_TILE_TYPES_HPP
#define OM_TILE_TYPES_HPP

#include "common/omGl.h"
#include "tiles/omTileCoord.h"

#include <boost/shared_ptr.hpp>
#include <deque>

class OmTextureID;
typedef boost::shared_ptr<OmTextureID> OmTextureIDPtr;

typedef struct {
	GLfloat x;
	GLfloat y;
} GLfloatPair;

typedef struct {
	GLfloatPair lowerLeft;
	GLfloatPair lowerRight;
	GLfloatPair upperRight;
	GLfloatPair upperLeft;
} GLfloatBox;

typedef struct {
	OmTileCoord tileCoord;
	GLfloatBox vertices;
} OmTileCoordAndVertices;

typedef struct {
	OmTilePtr tile;
	GLfloatBox vertices;
} OmTileAndVertices;

typedef std::deque<OmTileCoordAndVertices> OmTileCoordsAndLocations;
typedef boost::shared_ptr<OmTileCoordsAndLocations> OmTileCoordsAndLocationsPtr;

#endif
