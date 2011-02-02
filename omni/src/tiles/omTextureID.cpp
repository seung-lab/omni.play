#include "common/omDebug.h"
#include "tiles/cache/omTileCache.h"
#include "system/omGarbage.h"
#include "tiles/omTextureID.h"

OmTextureID::OmTextureID()
	: dims_(Vector2i(0,0))
	, flag_(OMTILE_COORDINVALID)
	, numBytes_(0)
{}

OmTextureID::OmTextureID(const Vector2i& dims,
						 boost::shared_ptr<uint8_t> data)
	: dims_(dims)
	, flag_(OMTILE_NEEDTEXTUREBUILT)
	, numBytes_(dims_.x * dims_.y * sizeof(uint8_t))
	, tileData_(data)
{}

OmTextureID::OmTextureID(const Vector2i& dims,
						 boost::shared_ptr<OmColorRGBA> data)
	: dims_(dims)
	, flag_(OMTILE_NEEDCOLORMAP)
	, numBytes_(dims_.x * dims_.y * sizeof(OmColorRGBA))
	, tileData_(data)
{}

OmTextureID::~OmTextureID()
{
	if(textureID_){
		OmGarbage::assignOmTextureId(*textureID_);
	}

	// data now in OpenGL texture; will be garbage collected via
	//  OmGarbage getting called from main GUI thread
}


class GetTileDataVisitor : public boost::static_visitor<void*>{
public:
	template <typename T>
	void* operator()(T & d ) const {
		return d.get();
	}
};
void* OmTextureID::GetTileData() const
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
