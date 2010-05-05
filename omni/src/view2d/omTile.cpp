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

	//debug("genone","OmTile::OmTile()");

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
	//debug("genone","OmTile::AddOverlay()");
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
	//std::cerr << "entering " << __FUNCTION__ << endl;

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
				OmIds myIdSet;

				textureID =
				    new OmTextureID(key, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y,
						    myIdSet, cache, vData, OMTILE_NEEDTEXTUREBUILT);
			} else {
				void * out = NULL;
				if (1 == mBytesPerSample) {
					uint32_t *vDataFake = NULL;
					vDataFake = (uint32_t*) malloc((tile_dims.x * tile_dims.y) * sizeof(SEGMENT_DATA_TYPE));
					//memset (vDataFake, '\0', (tile_dims.x * tile_dims.y) * sizeof (SEGMENT_DATA_TYPE));
					for (int i = 0; i < (tile_dims.x * tile_dims.y); i++) {
						vDataFake[i] = ((unsigned char *)(vData))[i];
					}
					OmIds myIdSet = setMyColorMap(((SEGMENT_DATA_TYPE *) vDataFake), tile_dims, key, &out);
					textureID = new OmTextureID(key, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y,
						    myIdSet, cache, out, OMTILE_NEEDCOLORMAP);
					free(vDataFake);
					free(vData);
				}
				OmIds myIdSet = setMyColorMap(((SEGMENT_DATA_TYPE *) vData), tile_dims, key, &out);
				textureID = new OmTextureID(key, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y,
						    myIdSet, cache, out, OMTILE_NEEDCOLORMAP);
				free(vData);
			}
			return textureID;
		}
	}
	// //debug("FIXME", << "MIP COORD IS INVALID" << endl;
	OmIds myIdSet;
	OmTextureID *textureID = new OmTextureID(key, 0, 0, 0, 0, myIdSet, NULL, NULL, OMTILE_COORDINVALID);
	//glDisable (GL_TEXTURE_2D); /* disable texture mapping */ 
	return textureID;
}

void *OmTile::GetImageData(const OmTileCoord & key, Vector2<int> &sliceDims, OmMipVolume * vol)
{
	//TODO: pull more data out when chunk is open

	//debug("genone","INSIDE HDF5 ERROR");

	QExplicitlySharedDataPointer < OmSimpleChunk > my_chunk;
	vol->GetSimpleChunk(my_chunk, TileToMipCoord(key));
	////debug("genone","after hdf5 error?");
	int mDepth = GetDepth(key);

	my_chunk->Open();

	int realDepth = mDepth % (vol->GetChunkDimension());

	void *void_data = NULL;
	if (view_type == XY_VIEW) {
		////debug("genone","realdepth: " << realDepth);
		////debug("genone","mipcoord: " << TileToMipCoord(key));
		void_data = my_chunk->ExtractDataSlice(VOL_XY_PLANE, realDepth, sliceDims, false);
	} else if (view_type == XZ_VIEW) {
		void_data = my_chunk->ExtractDataSlice(VOL_XZ_PLANE, realDepth, sliceDims, false);
	} else if (view_type == YZ_VIEW) {
		void_data = my_chunk->ExtractDataSlice(VOL_YZ_PLANE, realDepth, sliceDims, false);
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

	debug("tile", "factor:%f\n", factor);

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
	}
	return ret;
}

int clamp(int c)
{
	if (c > 255)
		return 255;
	return c;
}

OmIds OmTile::setMyColorMap(SEGMENT_DATA_TYPE * imageData, Vector2<int> dims, const OmTileCoord & key, void **rData)
{
	//debug("genone","OmTile::setMyColorMap(imageData)");
	debug("blank", "going to make it the texture now\n");

	OmIds found_ids;

	DataBbox data_bbox = mVolume->MipCoordToDataBbox(TileToMipCoord(key), 0);

	unsigned char *data = new unsigned char[dims.x * dims.y * SEGMENT_DATA_BYTES_PER_SAMPLE];

	int ctr = 0;
	int newctr = 0;

	//debug ("genone", "key volume type: %i\n", key.mVolType);

	OmSegmentation & current_seg = OmProject::GetSegmentation(myID);
	bool doValidate = current_seg.AreSegmentsSelected();
	if (SEGMENTATION == key.mVolType) {
		doValidate = false;	
	}

	QHash < SEGMENT_DATA_TYPE, QColor > speedTable;
	QColor newcolor;
	SEGMENT_DATA_TYPE lastid = 0;

	// looping through each value of imageData, which is strictly dims.x * dims.y big, no extra because of cast to SEGMENT_DATA_TYPE
	for (int i = 0; i < dims.x * dims.y; i++) {
		SEGMENT_DATA_TYPE tmpid = (SEGMENT_DATA_TYPE) imageData[i];

		if (tmpid != lastid) {
			if (!speedTable.contains(tmpid)) {

				//debug("FIXME", << "gotten segment id mapped to value" << endl;

				OmId id = current_seg.GetSegmentIdMappedToValue(tmpid);
				if (id == 0) {
					data[ctr] = 0;
					data[ctr + 1] = 0;
					data[ctr + 2] = 0;
					data[ctr + 3] = 255;
					newcolor = qRgba(0, 0, 0, 255);
				} else {

					found_ids.insert(id);

					// //debug("FIXME", << "asking for color now" << endl;
					OmSegment * segment = current_seg.GetSegment(id);
					const Vector3 < float >&color = segment->GetColor();

					if (current_seg.IsSegmentSelected(id))
						newcolor =
						    qRgba(clamp(color.x * 255 * 2.5), clamp(color.y * 255 * 2.5),
							  clamp(color.z * 255 * 2.5), 100);

					else {
						if (doValidate)
							newcolor = qRgba(0, 0, 0, 255);
						else
							newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);
					}

					data[ctr] = newcolor.red();
					data[ctr + 1] = newcolor.green();
					data[ctr + 2] = newcolor.blue();
					data[ctr + 3] = 255;
				}

				speedTable[tmpid] = newcolor;
				//debug("FIXME", << " adding to speed table" << endl;
			} else {
				//debug("FIXME", << " using speed table" << endl;
				newcolor = speedTable.value(tmpid);
				data[ctr] = newcolor.red();
				data[ctr + 1] = newcolor.green();
				data[ctr + 2] = newcolor.blue();
				data[ctr + 3] = 255;
			}
		} else {
			data[ctr] = newcolor.red();
			data[ctr + 1] = newcolor.green();
			data[ctr + 2] = newcolor.blue();
			data[ctr + 3] = 255;
		}
		newctr = newctr + 1;
		ctr = ctr + 4;
		lastid = tmpid;
	}
	debug("blank", "going to make it the texture now\n");
	*rData = data;

	return found_ids;
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

		OmId id = current_seg.GetSegmentIdMappedToValue(fg_voxel_value);
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
