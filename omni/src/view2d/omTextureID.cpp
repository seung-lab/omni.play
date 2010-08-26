#include "volume/omVolume.h"
#include "omTextureID.h"
#include "omThreadedCachingTile.h"
#include "system/omGarbage.h"
#include "common/omDebug.h"
#include "system/cache/omTileCache.h"

OmTextureID::OmTextureID(const OmTileCoord & tileCoord,
			 const Vector2i& dims,
			 OmTileCache * cache,
			 boost::shared_ptr<uint8_t> texture,
			 const OmTileFlag flag)
	: OmCacheableBase(cache)
	, mTileCoordinate(tileCoord)
	, textureID(0)
	, dims_(dims)
	, texture_(texture)
	, flag_(flag)
{
	mem_size = dims.x * dims.y * sizeof(uint8_t);
	UpdateSize(mem_size);
}

OmTextureID::OmTextureID(const OmTileCoord & tileCoord,
			 const Vector2i& dims,
			 OmTileCache * cache,
			 boost::shared_ptr<OmColorRGBA> texture,
			 const OmTileFlag flag)
	: OmCacheableBase(cache)
	, mTileCoordinate(tileCoord)
	, textureID(0)
	, dims_(dims)
	, texture_(texture)
	, flag_(flag)
{
	mem_size = dims.x * dims.y * sizeof(OmColorRGBA);
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
