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

#include "omTextureID.h"
#include "omTileCoord.h"
#include "common/omDebug.h"

#define DEBUG 0

OmTile::OmTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume * vol)
{
	view_type = viewtype;
	vol_type = voltype;

	myID = image_id;

	mVolume = vol;

	backgroundID = 0;

	mAlpha = OmPreferences::GetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT);
}

OmTile::~OmTile()
{
	//debug("genone","OmTile::~OmTile()");
}

void OmTile::AddOverlay(ObjectType secondtype, OmId second_id, OmMipVolume * secondvol)
{
	background_type = secondtype;
	backgroundID = second_id;
	mBackgroundVolume = secondvol;
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
			void *vBGData = NULL;
			if (backgroundID != 0) {
				mBackgroundSamplesPerVoxel = 1;
				mBackgroundBytesPerSample = mBackgroundVolume->GetBytesPerSample();
				vBGData = GetImageData(key, tile_bg_dims, mBackgroundVolume);
			}

			OmTextureID *textureID;

			if (vol_type == CHANNEL) {
				textureID =
				    new OmTextureID(key, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y,
						    cache, vData, OMTILE_NEEDTEXTUREBUILT);
			} else {
				void * out = NULL;
				if (1 == mBytesPerSample) {
					uint32_t *vDataFake = NULL;
					vDataFake = (uint32_t*) malloc((tile_dims.x * tile_dims.y) * sizeof(SEGMENT_DATA_TYPE));
					//memset (vDataFake, '\0', (tile_dims.x * tile_dims.y) * sizeof (SEGMENT_DATA_TYPE));
					for (int i = 0; i < (tile_dims.x * tile_dims.y); i++) {
						vDataFake[i] = ((unsigned char *)(vData))[i];
					}
					setMyColorMap(((SEGMENT_DATA_TYPE *) vDataFake), tile_dims, key, &out);
					textureID = new OmTextureID(key, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y,
						    cache, out, OMTILE_NEEDCOLORMAP);
					free(vDataFake);
					free(vData);
				}
				setMyColorMap(((SEGMENT_DATA_TYPE *) vData), tile_dims, key, &out);
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

void OmTile::setMyColorMap(SEGMENT_DATA_TYPE * imageData, Vector2<int> dims, const OmTileCoord & key, void **rData)
{
	unsigned char *data = new unsigned char[dims.x * dims.y * SEGMENT_DATA_BYTES_PER_SAMPLE];

	OmSegmentation & current_seg = OmProject::GetSegmentation(myID);
	
	current_seg.ColorTile( imageData, 
			       dims.x * dims.y,
			       key.mVolType,
			       data );

	*rData = data;
}

void OmTile::ReplaceTextureRegion(set < DataCoord > &vox)
{
	// so instead of relying on the color, i want to have *data be filled
	//   with the appropriate value from channel

	if (vox.empty()) {
		return;
	}

	unsigned char * data = new unsigned char[4];

	set < DataCoord >::iterator itr;

	OmSegmentation & current_seg = OmProject::GetSegmentation(myID);

	for (itr = vox.begin(); itr != vox.end(); itr++) {

		// data coord is flat xy view, need to translate into other views in order to access data

		DataCoord vox = *itr;

		DataCoord orthoVox;
		switch (view_type) {
		case XY_VIEW:
			orthoVox = DataCoord(vox.x, vox.y, vox.z);
			break;
		case XZ_VIEW:
			// ortho coord: (x, z, y) need (x, y, z)
			orthoVox = DataCoord(vox.x, vox.z, vox.y);
			break;
		case YZ_VIEW:
			// ortho coord: (z, y, x) need (x, y, z)
			orthoVox = DataCoord(vox.z, vox.x, vox.y);
			break;
		}

		//debug("FIXME", << "orthoVox = " << orthoVox << " ----- view = " << view_type << endl;
		// OmId id = current_seg.GetSegmentIdMappedToValue((SEGMENT_DATA_TYPE)
		uint32_t bg_voxel_value = mBackgroundVolume->GetVoxelValue(orthoVox);
		SEGMENT_DATA_TYPE fg_voxel_value = mVolume->GetVoxelValue(orthoVox);
		//debug("FIXME", << "BG VOXEL VALUE = " << bg_voxel_value << endl;
		//debug("FIXME", << "FG VOXEL VALUE = " << fg_voxel_value << endl;

		// okay so IF fg is 0, then all bg
		// if fg != 0, then mix

		OmId id = fg_voxel_value;
		QColor newcolor;

		if (id == 0) {
			data[0] = bg_voxel_value;
			data[1] = bg_voxel_value;
			data[2] = bg_voxel_value;
			data[3] = 255;
		} else {

			if (current_seg.IsSegmentSelected(id)) {

				switch (OmStateManager::GetSystemMode()) {
				case NAVIGATION_SYSTEM_MODE:
				case DEND_MODE:

					newcolor = qRgba(255, 255, 0, 255);

					data[0] =
						(newcolor.red() * .95) + ((bg_voxel_value) * (1.0 - .95));
					data[1] =
						(newcolor.green() * .95) + ((bg_voxel_value) * (1.0 - .95));
					data[2] =
						(newcolor.blue() * .95) + ((bg_voxel_value) * (1.0 - .95));
					data[3] = 255;
					
					break;

				case EDIT_SYSTEM_MODE:{
					OmSegment * segment = OmProject::GetSegmentation(myID).GetSegment(id);
					const Vector3 < float >&color = segment->GetColor();

					newcolor =
						qRgba(color.x * 255, color.y * 255, color.z * 255, 100);

					data[0] =
						(newcolor.red() * .95) + ((bg_voxel_value) * (1.0 - .95));
					data[1] =
						(newcolor.green() * .95) + ((bg_voxel_value) * (1.0 - .95));
					data[2] =
						(newcolor.blue() * .95) + ((bg_voxel_value) * (1.0 - .95));
					data[3] = 255;
				}

				}
			} else {
				OmSegment * segment = OmProject::GetSegmentation(myID).GetSegment(id);
				const Vector3 < float >&color = segment->GetColor();

				newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);

				data[0] = (newcolor.red() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
				data[1] = (newcolor.green() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
				data[2] = (newcolor.blue() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
				data[3] = 255;
			}
		}
	}
}
