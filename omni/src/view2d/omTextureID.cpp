#include "volume/omVolume.h"
#include "omTextureID.h"
#include "omThreadedCachingTile.h"
#include "system/omGarbage.h"
#include "common/omDebug.h"
#include "system/cache/omTileCache.h"

OmTextureID::OmTextureID()
	: OmCacheableBase(NULL)
	, textureID(0)
	, dims_(Vector2i(0,0))
	, flag_(OMTILE_COORDINVALID)
	, mem_size(0)
{
}

OmTextureID::OmTextureID(const Vector2i& dims, OmTileCache* cache)
	: OmCacheableBase(cache)
	, textureID(0)
	, dims_(dims)
	, flag_(OMTILE_COORDINVALID)
	, mem_size(0)
{
}

void OmTextureID::setData(boost::shared_ptr<uint8_t> data)
{
	flag_ = OMTILE_NEEDTEXTUREBUILT;
	mem_size = dims_.x * dims_.y * sizeof(uint8_t);

	tileData_ = data;
	UpdateSize(mem_size);
}

void OmTextureID::setData(boost::shared_ptr<OmColorRGBA> data)
{
	flag_ = OMTILE_NEEDCOLORMAP;
	mem_size = dims_.x * dims_.y * sizeof(OmColorRGBA);

	tileData_ = data;
	UpdateSize(mem_size);
}

OmTextureID::~OmTextureID()
{
	OmGarbage::assignOmTextureId(textureID);

	//remove object size from cache
	UpdateSize(-mem_size);
}


class GetTileDataVisitor : public boost::static_visitor<void*>{
public:
	template <typename T>
	void* operator()(T & d ) const {
		return d.get();
	}
};
void* OmTextureID::getTileData()
{
	return boost::apply_visitor(GetTileDataVisitor(),
				    tileData_);
}


class DeleteTileDataVisitor : public boost::static_visitor<>{
public:
	template <typename T>
	void operator()(T & d ) const {
		d.reset();
	}
};
void OmTextureID::deleteTileData()
{
	boost::apply_visitor(DeleteTileDataVisitor(),
			     tileData_);
}
