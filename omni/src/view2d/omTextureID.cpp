
#include "omTextureID.h"
#include "omThreadedCachingTile.h"
#include "system/omGarbage.h"
#include "common/omDebug.h"

#define DEBUG 0

OmTextureID::OmTextureID(const OmTileCoord & tileCoord, const GLuint & texID, const int &size, const int x, const int y,
			 const OmIds & containedIds, OmThreadedCachingTile * cache, void *texture, int flags)
:OmCacheableBase(cache), mTileCoordinate(tileCoord), textureID(texID), mem_size(size), mIdSet(containedIds),
texture(texture), flags(flags), x(x), y(y)
{
	debug("crazycash","texID %i\n", texID); 
	if (!cache) assert (0);

	//UpdateSize(mem_size);
	UpdateSize(128 * 128 * 4);
}

OmTextureID::~OmTextureID()
{
	//debug("genone","OmTextureID::~OmTextureID(%i)\n", textureID);

	//glDeleteTextures( 1, &textureID);
	//Attempt a safe delete of the gl texture id.
	OmGarbage::asOmTextureId(textureID);

	//remove object size from cache
	//UpdateSize(-mem_size);
	UpdateSize(-128 * 128 * 4);
}

const bool OmTextureID::FindId(OmId f_id)
{

	return (mIdSet.find(f_id) != mIdSet.end());
}

/////////////////////////////////
///////          ostream

ostream & operator<<(ostream & out, const OmTextureID & tid)
{

	out << "Texture ID: " << tid.GetTextureID() << "\n";
	out << "Size in Memory: " << tid.GetSize() << "\n";
	out << "Tile Coordinate Level: " << tid.GetCoordinate().Level << "\n";
}
