#include <stdlib.h>

#include "omTile.h"

#include "volume/omVolume.h"
#include "project/omProject.h"
#include "volume/omMipChunk.h"
#include "system/omStateManager.h"

#include "common/omStd.h"
#include "common/omGl.h"
#include "volume/omMipVolume.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include "system/viewGroup/omViewGroupState.h"

#include "omTextureID.h"
#include "omTileCoord.h"
#include "common/omDebug.h"

OmTile::OmTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume * vol, OmViewGroupState * vgs)
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
		return OmTextureIDPtr(new OmTextureID(key, 0, 0, 0, 0,
						      NULL, NULL,
						      OMTILE_COORDINVALID));
	}

	const int mcc_x = mMipChunkCoord.Coordinate.x;
	const int mcc_y = mMipChunkCoord.Coordinate.y;
	const int mcc_z = mMipChunkCoord.Coordinate.z;

	const bool legalCoord = (mcc_x >= 0) && (mcc_y >= 0) && (mcc_z >= 0);

	if(!legalCoord){
		return OmTextureIDPtr(new OmTextureID(key, 0, 0, 0, 0,
						      NULL, NULL,
						      OMTILE_COORDINVALID));
	}

	return doBindToTextureID(key, cache);
}

OmTextureIDPtr OmTile::doBindToTextureID(const OmTileCoord & key, OmTileCache* cache)
{
	mSamplesPerVoxel = 1;
	mBytesPerSample = mVolume->GetBytesPerSample();

	Vector2<int> tile_dims;
	void * vData = GetImageData(key, tile_dims, mVolume);

	OmTextureIDPtr textureID;

	const int dataSize = tile_dims.x * tile_dims.y;

	if (vol_type == CHANNEL) {
		textureID = OmTextureIDPtr(new OmTextureID(key, 0, dataSize, tile_dims.x, tile_dims.y,
							   cache, vData, OMTILE_NEEDTEXTUREBUILT));
		// don't free vData
	} else {
		void * out = NULL;

		if (1 == mBytesPerSample) {

			uint32_t * vDataFake = (uint32_t*) malloc( dataSize * sizeof(OmSegID));

			for (int i = 0; i < dataSize; ++i) {
				vDataFake[i] = ((unsigned char *)(vData))[i];
			}

			setMyColorMap(((OmSegID *) vDataFake), tile_dims, key, &out);

			textureID = OmTextureIDPtr(new OmTextureID(key, 0, dataSize, tile_dims.x, tile_dims.y,
								   cache, out, OMTILE_NEEDCOLORMAP));
			free(vDataFake);
		} else {
			setMyColorMap(((OmSegID *) vData), tile_dims, key, &out);
			textureID = OmTextureIDPtr(new OmTextureID(key, 0, 4*dataSize, tile_dims.x, tile_dims.y,
								   cache, out, OMTILE_NEEDCOLORMAP));
		}
		free(vData);
	}

	return textureID;
}

void * OmTile::GetImageData(const OmTileCoord & key, Vector2<int> &sliceDims, OmMipVolume * vol)
{
	OmMipChunkPtr my_chunk;
	vol->GetChunk(my_chunk, TileToMipCoord(key), true);

	const int mDepth = GetDepth(key);

	//my_chunk->Open();

	const int realDepth = mDepth % (vol->GetChunkDimension());
	sliceDims = my_chunk->GetSliceDims();
	return my_chunk->ExtractDataSlice(view_type, realDepth);
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

void OmTile::setMyColorMap(OmSegID * imageData, Vector2<int> dims, const OmTileCoord & key, void **rData)
{
	unsigned char *data = (unsigned char*) malloc(dims.x * dims.y * SEGMENT_DATA_BYTES_PER_SAMPLE);

	mViewGroupState->ColorTile( imageData,
				    dims.x * dims.y,
				    key.mVolType,
				    data );

	*rData = data;
}
