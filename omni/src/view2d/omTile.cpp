#include <stdlib.h>

#include "omTile.h"

#include "volume/omVolume.h"
#include "project/omProject.h"
#include "volume/omMipChunk.h"
#include "system/omStateManager.h"

#include "common/omStd.h"
#include "common/omGl.h"
#include "volume/omMipVolume.h"
#include "volume/omVolume.h"

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

OmTextureID *OmTile::BindToTextureID(const OmTileCoord & key, OmThreadedCachingTile* cache)
{
	OmMipChunkCoord mMipChunkCoord = TileToMipCoord(key);

	if (mVolume->ContainsMipChunkCoord(mMipChunkCoord)) {

		int mcc_x = mMipChunkCoord.Coordinate.x;
		int mcc_y = mMipChunkCoord.Coordinate.y;
		int mcc_z = mMipChunkCoord.Coordinate.z;

		if ((mcc_x >= 0) && (mcc_y >= 0) && (mcc_z >= 0)) {
			mSamplesPerVoxel = 1;
			mBytesPerSample = mVolume->GetBytesPerSample();
			Vector2<int> tile_dims;
			void *vData = NULL;

			vData = GetImageData(key, tile_dims, mVolume);
			//debug("FIXME", << "mBytesPerSample: " << mBytesPerSample << endl;;

			Vector2<int> tile_bg_dims;

			OmTextureID *textureID;

			if (vol_type == CHANNEL) {
				textureID =
				    new OmTextureID(key, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y,
						    cache, vData, OMTILE_NEEDTEXTUREBUILT);
			} else {
				void * out = NULL;
				if (1 == mBytesPerSample) {
					uint32_t *vDataFake = NULL;
					vDataFake = (uint32_t*) malloc((tile_dims.x * tile_dims.y) * sizeof(OmSegID));
					//memset (vDataFake, '\0', (tile_dims.x * tile_dims.y) * sizeof (OmSegID));
					for (int i = 0; i < (tile_dims.x * tile_dims.y); i++) {
						vDataFake[i] = ((unsigned char *)(vData))[i];
					}
					setMyColorMap(((OmSegID *) vDataFake), tile_dims, key, &out);
					textureID = new OmTextureID(key, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y,
						    cache, out, OMTILE_NEEDCOLORMAP);
					free(vDataFake);
					free(vData);
				}
				setMyColorMap(((OmSegID *) vData), tile_dims, key, &out);
				textureID = new OmTextureID(key, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y,
						    cache, out, OMTILE_NEEDCOLORMAP);
				free(vData);
			}
			return textureID;
		}
	}

	OmTextureID *textureID = new OmTextureID(key, 0, 0, 0, 0, NULL, NULL, OMTILE_COORDINVALID);
	//glDisable (GL_TEXTURE_2D); /* disable texture mapping */ 
	return textureID;
}

void *OmTile::GetImageData(const OmTileCoord & key, Vector2<int> &sliceDims, OmMipVolume * vol)
{
	//TODO: pull more data out when chunk is open

	QExplicitlySharedDataPointer < OmSimpleChunk > my_chunk;
	vol->GetSimpleChunk(my_chunk, TileToMipCoord(key));

	int mDepth = GetDepth(key);

	my_chunk->Open();

	int realDepth = mDepth % (vol->GetChunkDimension());

	void *void_data = NULL;
	switch( view_type ){
	case XY_VIEW:
		void_data = my_chunk->ExtractDataSlice(VOL_XY_PLANE, realDepth, sliceDims, false);
		break;
	case XZ_VIEW:
		void_data = my_chunk->ExtractDataSlice(VOL_XZ_PLANE, realDepth, sliceDims, false);
		break;
	case YZ_VIEW:
		void_data = my_chunk->ExtractDataSlice(VOL_YZ_PLANE, realDepth, sliceDims, false);
		break;
	}

	return void_data;
}

OmMipChunkCoord OmTile::TileToMipCoord(const OmTileCoord & key)
{
	// find mip coord
	OmSegmentation & current_seg = OmProject::GetSegmentation(myID);
	NormCoord mNormCoord = current_seg.SpaceToNormCoord(key.Coordinate);
	return mVolume->NormToMipCoord(mNormCoord, key.Level);
}

int OmTile::GetDepth(const OmTileCoord & key)
{
	OmSegmentation & current_seg = OmProject::GetSegmentation(myID);
	NormCoord normCoord = current_seg.SpaceToNormCoord(key.Coordinate);
	DataCoord dataCoord = current_seg.NormToDataCoord(normCoord);
        float factor=OMPOW(2,key.Level);

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
	unsigned char *data = new unsigned char[dims.x * dims.y * SEGMENT_DATA_BYTES_PER_SAMPLE];

	mViewGroupState->ColorTile( imageData, 
				    dims.x * dims.y,
				    key.mVolType,
				    data );
	
	*rData = data;
}
