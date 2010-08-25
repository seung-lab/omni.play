#include "volume/omVolume.h"
#include "omTextureID.h"
#include "omThreadedCachingTile.h"
#include "system/omGarbage.h"
#include "common/omDebug.h"
#include "system/cache/omTileCache.h"

OmTextureID::OmTextureID(const OmTileCoord & tileCoord, const GLuint & texID,
			 const int &size, const int x, const int y,
			 OmTileCache * cache, void *texture,
			 int flags)
	: OmCacheableBase(cache)
	, mTileCoordinate(tileCoord)
	, textureID(texID)
	, mem_size(size)
	, texture(texture)
	, flags(flags)
	, x(x)
	, y(y)
{
	UpdateSize(mem_size);
}

OmTextureID::~OmTextureID()
{
	OmGarbage::asOmTextureId(textureID);
	if(texture) {
		//printf("freeing texture that was never displayed\n");
		free(texture);
	}

	//remove object size from cache
	UpdateSize(-mem_size);
}
