#include "volume/omVolume.h"
#include "omTextureID.h"
#include "omThreadedCachingTile.h"
#include "system/omGarbage.h"
#include "common/omDebug.h"
#include "system/cache/omTileCache.h"

OmTextureID::OmTextureID(const OmTileCoord & tileCoord, const GLuint & texID,
			 const int &size, const int x, const int y,
			 OmTileCache * cache,
			 boost::shared_ptr<uint8_t> texture,
			 int flags)
	: OmCacheableBase(cache)
	, mTileCoordinate(tileCoord)
	, textureID(texID)
	, mem_size(size)
	, texture_(texture)
	, flags(flags)
	, x(x)
	, y(y)
{
	UpdateSize(mem_size);
}

OmTextureID::OmTextureID(const OmTileCoord & tileCoord, const GLuint & texID,
			 const int &size, const int x, const int y,
			 OmTileCache * cache,
			 boost::shared_ptr<uint32_t> texture,
			 int flags)
	: OmCacheableBase(cache)
	, mTileCoordinate(tileCoord)
	, textureID(texID)
	, mem_size(size)
	, texture_(texture)
	, flags(flags)
	, x(x)
	, y(y)
{
	UpdateSize(mem_size);
}

OmTextureID::~OmTextureID()
{
	OmGarbage::asOmTextureId(textureID);

	//remove object size from cache
	UpdateSize(-mem_size);
}


class GetTextureVisitor : public boost::static_visitor<void*>{
public:
	template <typename T>
	void* operator()(T & d ) const {
		return d.get();
	}
};
void* OmTextureID::getTexture()
{
	return boost::apply_visitor(GetTextureVisitor(),
				    texture_);
}


class DeleteTextureVisitor : public boost::static_visitor<>{
public:
	template <typename T>
	void operator()(T & d ) const {
		d.reset();
	}
};
void OmTextureID::deleteTexture()
{
	boost::apply_visitor(DeleteTextureVisitor(),
			     texture_);
}
