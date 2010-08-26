#include <stdlib.h>
\
#include "common/omDebug.h"
#include "common/omGl.h"
#include "common/omStd.h"
#include "omTextureID.h"
#include "omTile.h"
#include "omTileCoord.h"
#include "project/omProject.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "volume/omMipChunk.h"
#include "volume/omMipVolume.h"
#include "volume/omVolume.h"
#include "volume/omVolumeData.hpp"

OmTile::OmTile(ViewType viewtype, ObjectType voltype, OmId image_id,
	       OmMipVolume * vol, OmViewGroupState * vgs)
{
	view_type = viewtype;
	vol_type = voltype;

	myID = image_id;

	mVolume = vol;

	mViewGroupState = vgs;

	mAlpha = OmPreferences::GetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT);
}

OmTile::~OmTile()
{
	//debug("genone","OmTile::~OmTile()");
}

void OmTile::SetNewAlpha(float newval)
{
	mAlpha = newval;
}

OmTextureIDPtr OmTile::BindToTextureID(const OmTileCoord & key, OmTileCache* cache)
{
	OmMipChunkCoord mMipChunkCoord = TileToMipCoord(key);

	if(!mVolume->ContainsMipChunkCoord(mMipChunkCoord)) {
		return makeNullTextureID(key);
	}

	const int mcc_x = mMipChunkCoord.Coordinate.x;
	const int mcc_y = mMipChunkCoord.Coordinate.y;
	const int mcc_z = mMipChunkCoord.Coordinate.z;

	const bool legalCoord = (mcc_x >= 0) && (mcc_y >= 0) && (mcc_z >= 0);

	if(!legalCoord){
		return makeNullTextureID(key);
	}

	return doBindToTextureID(key, cache);
}

OmTextureIDPtr OmTile::makeNullTextureID(const OmTileCoord& key)
{
	return OmTextureIDPtr(new OmTextureID(key,
					      Vector2i(0,0),
					      NULL,
					      boost::shared_ptr<uint8_t>(),
					      OMTILE_COORDINVALID));
}

OmTextureIDPtr OmTile::doBindToTextureID(const OmTileCoord & key, OmTileCache* cache)
{
	const Vector2i tile_dims(128,128);

	if (vol_type == CHANNEL) {
		boost::shared_ptr<uint8_t> vData = GetImageData8bit(key, mVolume);
		return OmTextureIDPtr(new OmTextureID(key,
						      tile_dims,
						      cache,
						      vData,
						      OMTILE_NEEDTEXTUREBUILT));
	}

	boost::shared_ptr<uint32_t> vData =
		GetImageData32bit(key, mVolume);

	boost::shared_ptr<uchar> colorMappedData =
		setMyColorMap(vData, tile_dims, key);

	return OmTextureIDPtr(new OmTextureID(key,
					      tile_dims,
					      cache,
					      colorMappedData,
					      OMTILE_NEEDCOLORMAP));
}


boost::shared_ptr<uint8_t> OmTile::GetImageData8bit(const OmTileCoord& key,
						    OmMipVolume* vol)
{
	OmMipChunkPtr my_chunk;
	vol->GetChunk(my_chunk, TileToMipCoord(key), true);

	const int mDepth = GetDepth(key);
	const int realDepth = mDepth % (vol->GetChunkDimension());
	return my_chunk->ExtractDataSlice8bit(view_type, realDepth);
}

boost::shared_ptr<uint32_t> OmTile::GetImageData32bit(const OmTileCoord& key,
						      OmMipVolume* vol)
{
	OmMipChunkPtr my_chunk;
	vol->GetChunk(my_chunk, TileToMipCoord(key), true);

	const int mDepth = GetDepth(key);
	const int realDepth = mDepth % (vol->GetChunkDimension());
	return my_chunk->ExtractDataSlice32bit(view_type, realDepth);
}

OmMipChunkCoord OmTile::TileToMipCoord(const OmTileCoord & key)
{
	// find mip coord
	NormCoord normCoord = mVolume->SpaceToNormCoord(key.Coordinate);
	return mVolume->NormToMipCoord(normCoord, key.Level);
}

int OmTile::GetDepth(const OmTileCoord & key)
{
	NormCoord normCoord = mVolume->SpaceToNormCoord(key.Coordinate);
	DataCoord dataCoord = mVolume->NormToDataCoord(normCoord);
        const float factor = OMPOW(2,key.Level);

	int ret;

	switch(view_type){
	case XY_VIEW:
		ret = (int)(dataCoord.z/factor);
		break;
	case XZ_VIEW:
		ret = (int)(dataCoord.y/factor);
		break;
	case YZ_VIEW:
		ret = (int)(dataCoord.x/factor);
		break;
	default:
		assert(0);
	}

	return ret;
}

boost::shared_ptr<uchar> OmTile::setMyColorMap(boost::shared_ptr<uint32_t> imageData,
					       const Vector2i& dims,
					       const OmTileCoord& key)
{
	const int numElements = dims.x * dims.y;
	const uint32_t memSize = numElements * 4; // 4 == RGBA
	boost::shared_ptr<uint8_t> colorMappedData
		= OmSmartPtr<uint8_t>::makeMallocPtr(memSize);

	mViewGroupState->ColorTile(imageData,
				   numElements,
				   key.mVolType,
				   colorMappedData);

	return colorMappedData;
}
