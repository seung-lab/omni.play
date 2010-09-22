#include <stdlib.h>

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
	: mVolume(vol)
	, dims_(Vector2i(128,128))
	, mViewGroupState(vgs)
	, view_type(viewtype)
	, vol_type(voltype)
	, myID(image_id)
{
	mAlpha = OmPreferences::GetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT);
}

OmTile::~OmTile()
{
}

void OmTile::SetNewAlpha(const float newval)
{
	mAlpha = newval;
}

OmTextureIDPtr OmTile::BindToTextureID(const OmTileCoord & key, OmTileCache* cache)
{
	OmMipChunkCoord mMipChunkCoord = TileToMipCoord(key);

	if(!mVolume->ContainsMipChunkCoord(mMipChunkCoord)) {
		return makeNullTextureID();
	}

	const int mcc_x = mMipChunkCoord.Coordinate.x;
	const int mcc_y = mMipChunkCoord.Coordinate.y;
	const int mcc_z = mMipChunkCoord.Coordinate.z;

	const bool legalCoord = (mcc_x >= 0) && (mcc_y >= 0) && (mcc_z >= 0);

	if(!legalCoord){
		return makeNullTextureID();
	}

	return doBindToTextureID(key, cache);
}

OmTextureIDPtr OmTile::makeNullTextureID()
{
	return OmTextureIDPtr(new OmTextureID());
}

OmTextureIDPtr OmTile::doBindToTextureID(const OmTileCoord & key,
					 OmTileCache* cache)
{
	OmTextureIDPtr tid(new OmTextureID(dims_, cache));

	if (vol_type == CHANNEL) {
		boost::shared_ptr<uint8_t> vData = GetImageData8bit(key);
		tid->setData(vData);
	} else {
		boost::shared_ptr<uint32_t> imageData = GetImageData32bit(key);
		boost::shared_ptr<OmColorRGBA> colorMappedData =
			mViewGroupState->ColorTile(imageData,
						   dims_,
						   key.mVolType);
		tid->setData(colorMappedData);
	}

	return tid;
}

int OmTile::getVolDepth(const OmTileCoord& key)
{
	const int mDepth = GetDepth(key);
	return mDepth % (mVolume->GetChunkDimension());
}

boost::shared_ptr<uint8_t> OmTile::GetImageData8bit(const OmTileCoord& key)
{
	OmMipChunkPtr chunk;
	mVolume->GetChunk(chunk, TileToMipCoord(key), true);

	return chunk->ExtractDataSlice8bit(view_type,
					   getVolDepth(key));
}

boost::shared_ptr<uint32_t> OmTile::GetImageData32bit(const OmTileCoord& key)
{
	OmMipChunkPtr chunk;
	mVolume->GetChunk(chunk, TileToMipCoord(key), true);

	return chunk->ExtractDataSlice32bit(view_type,
					    getVolDepth(key));
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
