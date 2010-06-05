
#include "volume/omVolume.h"
#include "omTextureID.h"
#include "omThreadedCachingTile.h"
#include "system/omGarbage.h"
#include "common/omDebug.h"

#define DEBUG 0

OmTextureID::OmTextureID(const OmTileCoord & tileCoord, const GLuint & texID, const int &size, const int x, const int y,
			 OmThreadedCachingTile * cache, void *texture, int flags)
	: OmCacheableBase(cache), 
	  mTileCoordinate(tileCoord), 
	  textureID(texID), 
	  mem_size(size),
	  texture(texture), 
	  flags(flags), 
	  x(x), 
	  y(y)
{
	if (!cache){
		assert (0);
	}

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

void OmTextureID::Flush()
{
	printf("FIXME: should I write something to disk?\n");
}
