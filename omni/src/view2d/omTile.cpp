
#include <stdlib.h>

#include "omTile.h"

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

#define DEBUG 0

OmTile::OmTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol) {
	
	DOUT("OmTile::OmTile()");
	
	view_type = viewtype;
	vol_type = voltype;
	
	myID = image_id;
	
	mVolume = vol;
	
	backgroundID = 0;
	
	mAlpha = OmPreferences::GetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT);
}


OmTile::~OmTile() {
	DOUT("OmTile::~OmTile()");
}

void OmTile::AddOverlay(ObjectType secondtype, OmId second_id, OmMipVolume *secondvol) {
	DOUT("OmTile::AddOverlay()");
	background_type = secondtype;
	backgroundID = second_id;
	mBackgroundVolume = secondvol;
}

void OmTile::SetNewAlpha(float newval) {
	mAlpha = newval;
}

OmTextureID* OmTile::BindToTextureID(const OmTileCoord &key) {
	//std::cerr << "entering " << __FUNCTION__ << endl;
	
	OmMipChunkCoord mMipChunkCoord = TileToMipCoord(key);
	
	if(mVolume->ContainsMipChunkCoord(mMipChunkCoord)) {
		
		int mcc_x = mMipChunkCoord.Coordinate.x;
		int mcc_y = mMipChunkCoord.Coordinate.y;
		int mcc_z = mMipChunkCoord.Coordinate.z;
		
		
		if((mcc_x >= 0) && (mcc_y >= 0) && (mcc_z >= 0)) {
			mSamplesPerVoxel = 1;
			mBytesPerSample = mVolume->GetBytesPerSample();
			Vector2<int> tile_dims;
			void *vData = NULL;

			vData = GetImageData(key, tile_dims, mVolume);
			//cout << "mBytesPerSample: " << mBytesPerSample << endl;;
			
			Vector2<int> tile_bg_dims;
			void* vBGData = NULL;
			if (backgroundID != 0) {
				mBackgroundSamplesPerVoxel = 1;
				mBackgroundBytesPerSample = mBackgroundVolume->GetBytesPerSample();
				vBGData = GetImageData(key, tile_bg_dims, mBackgroundVolume);			
			}
			
			OmTextureID *textureID;
			
			if (vol_type == CHANNEL) {
				OmIds myIdSet;
				
				textureID = new OmTextureID(key, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y, myIdSet, NULL, vData, OMTILE_NEEDTEXTUREBUILT);
			}
			else {
#if 0
				if  (1 == mBytesPerSample) {
				 	uint32_t *vDataFake;
					vDataFake = (uint32_t *) malloc ((tile_dims.x * tile_dims.y) * sizeof (SEGMENT_DATA_TYPE)); 
					//memset (vDataFake, '\0', (tile_dims.x * tile_dims.y) * sizeof (SEGMENT_DATA_TYPE));
					for (int i = 0; i < (tile_dims.x * tile_dims.y); i++) {
						vDataFake[i] = ((unsigned char*)(vData))[i];
						vDataFake[i] << 8;
						//cout << " "  << (int)((unsigned char*)(vData))[i];
					}
					cout << endl;
					free (vData);
					vData = (void *) vDataFake;
				}
				void *out = NULL;
				//cout << "in: vData: " << vData << ". " << out << endl;
				OmIds myIdSet = setMyColorMap(((SEGMENT_DATA_TYPE *) vData), tile_dims, key, &out);
				//cout << "out: vData: " << out << endl;
				vData = out;
#else
				OmIds myIdSet;
#endif
				textureID = new OmTextureID(key, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y, myIdSet, NULL, vData, OMTILE_NEEDCOLORMAP);
			}
			return textureID;
		}
	}
	// cout << "MIP COORD IS INVALID" << endl;
	OmIds myIdSet;
	OmTextureID *textureID = new OmTextureID(key, 0, 0, 0, 0, myIdSet, NULL, NULL, OMTILE_COORDINVALID);
	//glDisable (GL_TEXTURE_2D); /* disable texture mapping */ 
	return textureID;
}

void* OmTile::GetImageData(const OmTileCoord &key, Vector2<int> &sliceDims, OmMipVolume *vol) {
	//TODO: pull more data out when chunk is open
	
	//DOUT("INSIDE HDF5 ERROR");
	
	shared_ptr<OmMipChunk> my_chunk = vol->GetChunk(TileToMipCoord(key));
	//DOUT("after hdf5 error?");
	int mDepth = GetDepth(key, vol);
	
	my_chunk->Open ();
	
	return NULL;
	
	int realDepth = mDepth % (vol->GetChunkDimension());
	
	
	//DOUT("INSIDE DEPTH ERROR");
	void* void_data = NULL;
	if (view_type == XY_VIEW) {
		//DOUT("realdepth: " << realDepth);
		//DOUT("mipcoord: " << TileToMipCoord(key));
		void_data = my_chunk->ExtractDataSlice(VOL_XY_PLANE, realDepth, sliceDims, false);
	}
	else if (view_type == XZ_VIEW) {
		void_data = my_chunk->ExtractDataSlice(VOL_XZ_PLANE, realDepth, sliceDims, false);
	}
	else if (view_type == YZ_VIEW) {
		void_data = my_chunk->ExtractDataSlice(VOL_YZ_PLANE, realDepth, sliceDims, false);
	}
	
	//DOUT("after depth error?");
	
	// unsigned char* data = (unsigned char*)void_data;
	
	return void_data;
}


OmMipChunkCoord OmTile::TileToMipCoord(const OmTileCoord &key) {
	
	// find mip coord
	NormCoord mNormCoord = OmVolume::SpaceToNormCoord(key.Coordinate);
	return mVolume->NormToMipCoord(mNormCoord, key.Level);
	
}


int OmTile::GetDepth(const OmTileCoord &key, OmMipVolume *vol) {
	
	// find depth
	NormCoord mNormCoord = OmVolume::SpaceToNormCoord(key.Coordinate);
	DataCoord mDataCoord = OmVolume::NormToDataCoord(mNormCoord);
	
	if (view_type == XY_VIEW) {
		return mDataCoord.z;
	}
	if (view_type == XZ_VIEW) {
		return mDataCoord.y;
	}
	if (view_type == YZ_VIEW) {
		return mDataCoord.x;
	}
}

void OmTile::setMergeChannels(unsigned char *imageData, unsigned char *secondImageData, Vector2<int> dims, Vector2<int> second_dims, const OmTileCoord &key)
{
	DOUT("OmTile::setMergeChannels()");
	// imageData is channel data if there is a background volume
	
	DataBbox data_bbox = mVolume->MipCoordToDataBbox(TileToMipCoord(key), 0);
	int my_depth = GetDepth(key, mVolume);
	
	unsigned char * data = new unsigned char[dims.x*dims.y*4];
	
	
	int ctr = 0;
	int newctr = 0;
	
	int z_min = data_bbox.getMin().z;
	int z_max = data_bbox.getMax().z;
	int y_min = data_bbox.getMin().y;
	int y_max = data_bbox.getMax().y;
	int x_min = data_bbox.getMin().x;
	int x_max = data_bbox.getMax().x;
	if(view_type == XY_VIEW) {
		z_min = my_depth;
		z_max = my_depth;
	}
	else if (view_type == XZ_VIEW) {
		y_min = my_depth;
		y_max = my_depth;
	}
	else if (view_type == YZ_VIEW) {
		x_min = my_depth;
		x_max = my_depth;
	}
	
	//	cout << "IMAGE DATA: " << endl;
	//	for(int i = 0 ; i < 100 ; i++)
	//		cout << (unsigned int) imageData[i] << endl;
	//	
	//	cout << "SECOND DATA: " << endl;
	//	for(int i = 0 ; i < 100 ; i++)
	//		cout << (unsigned int) secondImageData[i] << endl;
	
	for (int z = z_min ; z <= z_max ; z++) {
		for (int y = y_min ; y <= y_max ; y++) {
			for (int x = x_min; x <= x_max ; x++) {
				
				data[ctr] = (unsigned int) imageData[newctr];
				data[ctr+1] =  (unsigned int) imageData[newctr];
				data[ctr+2] =  (unsigned int) imageData[newctr];
				data[ctr+3] = 255;
				
			}
			newctr = newctr + 1;
			ctr = ctr+4;
		}
	}
	
	
	
	//glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, dims.x, dims.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	
	
}


int clamp (int c)
{
        if (c > 255) return 255;
        return c;
}

OmIds OmTile::setMyColorMap(SEGMENT_DATA_TYPE *imageData, Vector2<int> dims, const OmTileCoord &key, void **rData)
{
	DOUT("OmTile::setMyColorMap(imageData)");
	
	OmIds found_ids;
	
	DataBbox data_bbox = mVolume->MipCoordToDataBbox(TileToMipCoord(key), 0);
	int my_depth = GetDepth(key, mVolume);
	
	unsigned char *data = new unsigned char[dims.x*dims.y*SEGMENT_DATA_BYTES_PER_SAMPLE];
	
	int ctr = 0;
	int newctr = 0;
	
	
	OmSegmentation &current_seg = OmVolume::GetSegmentation(myID);

        map<SEGMENT_DATA_TYPE,QColor> speedTable;
	QColor newcolor;
	SEGMENT_DATA_TYPE lastid = 0;
	
	// looping through each value of imageData, which is strictly dims.x * dims.y big, no extra because of cast to SEGMENT_DATA_TYPE
	for (int i = 0 ; i < dims.x * dims.y ; i++) {
		SEGMENT_DATA_TYPE tmpid = (SEGMENT_DATA_TYPE) imageData[i];
        	map<SEGMENT_DATA_TYPE,QColor>::iterator it = speedTable.find(tmpid);

		if (tmpid != lastid) {
			if (speedTable.end () == it) {

				//cout << "gotten segment id mapped to value" << endl;
			
				OmId id = current_seg.GetSegmentIdMappedToValue(tmpid);
				if(id == 0) {
					data[ctr] = 0;
					data[ctr+1] =  0;
					data[ctr+2] =  0;
					data[ctr+3] = 255;
					newcolor = qRgba(0,0,0,255);
				} else {
				
					found_ids.insert(id);
			
					// cout << "asking for color now" << endl;
					const Vector3<float> &color = OmVolume::GetSegmentation(myID).GetSegment(id).GetColor();
			
					if(current_seg.IsSegmentSelected(id))
                                                newcolor = qRgba(clamp (color.x * 255*2.5), clamp(color.y * 255*2.5), clamp(color.z * 255*2.5), 100);
				
					else	
						newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);
			
					data[ctr] = newcolor.red();
					data[ctr+1] = newcolor.green();
					data[ctr+2] = newcolor.blue();
					data[ctr+3] = 255;
				}

                        	speedTable[tmpid] = newcolor;
                        	//cout << " adding to speed table" << endl;
			} else { 
                        	//cout << " using speed table" << endl;
                        	newcolor = (*it).second;
                        	data[ctr] = newcolor.red();
                        	data[ctr+1] = newcolor.green();
                        	data[ctr+2] = newcolor.blue();
                        	data[ctr+3] = 255;
			}
		} else {
                       	data[ctr] = newcolor.red();
                       	data[ctr+1] = newcolor.green();
                       	data[ctr+2] = newcolor.blue();
                       	data[ctr+3] = 255;
		}
		newctr = newctr + 1;
		ctr = ctr+4;
		lastid = tmpid;
	}
	// cout << "going to make it the texture now" << endl;
	*rData = data;
	
	return found_ids;
}


OmIds OmTile::setMyColorMap(SEGMENT_DATA_TYPE *imageData, unsigned char *secondImageData, Vector2<int> dims, Vector2<int> second_dims, const OmTileCoord &key)
{
	//	DOUT("OmTile::setMyColorMap(imageData, secondImageData)");
	// secondImageData is channel data if there is a background volume
	
	OmIds found_ids;
	
	unsigned char * data = new unsigned char[dims.x*dims.y*4];
	
	//	cout << "SEG DIMS = " << dims << endl;
	//	cout << "CHAN DIMS = " << second_dims << endl;
	
	
	int ctr = 0;
	int newctr = 0;
	
	
	OmSegmentation &current_seg = OmVolume::GetSegmentation(myID);
	
	
	for (int i = 0 ; i < dims.x * dims.y ; i++) {
		
		OmId id = current_seg.GetSegmentIdMappedToValue((SEGMENT_DATA_TYPE) imageData[i]);
		
		// cout << "gotten segment id mapped to value" << endl;
		// DOUT("ID ID ID = " << id);
		QColor newcolor;
		if(id == 0) {
			data[ctr] = secondImageData[i];
			data[ctr+1] =  secondImageData[i];
			data[ctr+2] =  secondImageData[i];
			data[ctr+3] = 255;
		}
		else {
			
			found_ids.insert(id);
			
			if(current_seg.IsSegmentSelected(id)) {
				
				
				switch(OmStateManager::GetSystemMode()) {
					case NAVIGATION_SYSTEM_MODE: {
						newcolor = qRgba(255,255,0,255);
						
						//unsigned int dest = ((unsigned int) secondImageData[newctr]);
						unsigned char dest = secondImageData[i];
						
						data[ctr] = (newcolor.red() * .95) + ((dest) * (1.0 - .95));
						data[ctr+1] = (newcolor.green() * .95) + ((dest) * (1.0 - .95));
						data[ctr+2] = (newcolor.blue() * .95) + ((dest) * (1.0 - .95));
						data[ctr+3] = 255;
					}
						break;
						
					case EDIT_SYSTEM_MODE: {
						const Vector3<float> &color = OmVolume::GetSegmentation(myID).GetSegment(id).GetColor();
						
						newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);
						
						//unsigned int dest = ((unsigned int) secondImageData[newctr]);
						unsigned char dest = secondImageData[i];
						
						data[ctr] = (newcolor.red() * .95) + ((dest) * (1.0 - .95));
						data[ctr+1] = (newcolor.green() * .95) + ((dest) * (1.0 - .95));
						data[ctr+2] = (newcolor.blue() * .95) + ((dest) * (1.0 - .95));
						data[ctr+3] = 255;
					}
						
						const Vector3<float> &color = OmVolume::GetSegmentation(myID).GetSegment(id).GetColor();
						
						
				}
				
				
			}
			else {
				
				const Vector3<float> &color = OmVolume::GetSegmentation(myID).GetSegment(id).GetColor();
				
				newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);
				
				//unsigned int dest = ((unsigned int) secondImageData[newctr]);
				unsigned char dest = secondImageData[i];
				
				data[ctr] = (newcolor.red() * mAlpha) + ((dest) * (1.0 - mAlpha));
				data[ctr+1] = (newcolor.green() * mAlpha) + ((dest) * (1.0 - mAlpha));
				data[ctr+2] = (newcolor.blue() * mAlpha) + ((dest) * (1.0 - mAlpha));
				data[ctr+3] = 255;
			}
			
		}
		newctr = newctr + 1;
		ctr = ctr+4;
		
	}
	// cout << "going to make texture now" << endl;
	
	
	//glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, dims.x, dims.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	return found_ids;
	
}

void OmTile::ReplaceFullTextureRegion(shared_ptr<OmTextureID> &texID, DataCoord firstCoord, int tl) {
	DOUT("OmTile::ReplaceFullTextureRegion()");
	
	GLuint texture = texID->GetTextureID();
	//glBindTexture (GL_TEXTURE_2D, texture);
	
	unsigned char * data = new unsigned char[tl*tl*4];
	
	//	cout << "SEG DIMS = " << dims << endl;
	//	cout << "CHAN DIMS = " << second_dims << endl;
	
	
	int ctr = 0;
	int newctr = 0;
	
	
	OmSegmentation &current_seg = OmVolume::GetSegmentation(myID);
	
	// data coord is flat xy view, need to translate into other views in order to access data
	
	
	//	for (int i = 0 ; i < tl * tl ; i++) {
	
	if(view_type != YZ_VIEW) {
		
	for(int y = firstCoord.y ; y < firstCoord.y + tl ; y++) {
		for(int x = firstCoord.x ; x < firstCoord.x + tl ; x++) {
			
//			uint32_t bg_voxel_value = mBackgroundVolume->GetVoxelValue(DataCoord(x, y, firstCoord.z));
//			SEGMENT_DATA_TYPE fg_voxel_value = mVolume->GetVoxelValue(DataCoord(x, y, firstCoord.z));
			
			uint32_t bg_voxel_value;
			SEGMENT_DATA_TYPE fg_voxel_value;
			
			
			switch(view_type) {
				case XY_VIEW: {
					
					//DOUT("datacoord = " << DataCoord(x, y, firstCoord.z));
					bg_voxel_value = mBackgroundVolume->GetVoxelValue(DataCoord(x, y, firstCoord.z));
					fg_voxel_value = mVolume->GetVoxelValue(DataCoord(x, y, firstCoord.z));

				}
					break;
				case XZ_VIEW: {
					//DOUT("datacoord = " << DataCoord(x, firstCoord.z, y));
					bg_voxel_value = mBackgroundVolume->GetVoxelValue(DataCoord(x, firstCoord.z, y));
					fg_voxel_value = mVolume->GetVoxelValue(DataCoord(x, firstCoord.z, y));

				}
					break;
			}
			
//			DOUT("BG VOXEL VALUE = " << bg_voxel_value);
//			DOUT("FG VOXEL VALUE = " << fg_voxel_value);
			
			// okay so IF fg is 0, then all bg
			// if fg != 0, then mix
			
			OmId id = current_seg.GetSegmentIdMappedToValue(fg_voxel_value);
			QColor newcolor;
			
			if(id == 0) {
				data[ctr] = bg_voxel_value;
				data[ctr+1] = bg_voxel_value;
				data[ctr+2] = bg_voxel_value;
				data[ctr+3] = 255;
			}
			else {
				
				if(current_seg.IsSegmentSelected(id)) {
					
					switch(OmStateManager::GetSystemMode()) {
						case NAVIGATION_SYSTEM_MODE: {
							newcolor = qRgba(255,255,0,255);
							
							data[ctr] = (newcolor.red() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[ctr+1] = (newcolor.green() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[ctr+2] = (newcolor.blue() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[ctr+3] = 255;
						}
							break;
							
						case EDIT_SYSTEM_MODE: {
							const Vector3<float> &color = OmVolume::GetSegmentation(myID).GetSegment(id).GetColor();
							
							newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);
							
							data[ctr] = (newcolor.red() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[ctr+1] = (newcolor.green() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[ctr+2] = (newcolor.blue() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[ctr+3] = 255;
						}
							
					}
				}
				else {
					
					const Vector3<float> &color = OmVolume::GetSegmentation(myID).GetSegment(id).GetColor();
					
					newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);
					
					data[ctr] = (newcolor.red() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
					data[ctr+1] = (newcolor.green() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
					data[ctr+2] = (newcolor.blue() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
					data[ctr+3] = 255;
				}
			}
			
			// DataCoord localDataClickPoint = DataCoord(yzDataClickPoint.x % tileLength, yzDataClickPoint.y % tileLength, 0);
			
			int xcoord = (x % tl);
			int ycoord = (y % tl);
			
			// DOUT("x, y: = " << xcoord << ", " << ycoord);
			
			//glTexSubImage2D (GL_TEXTURE_2D, 0, xcoord, ycoord, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
			
			//ctr = ctr+4;
		}	
	}
	}
	else {
		
		/*
		 case YZ_VIEW: {
		 bg_voxel_value = mBackgroundVolume->GetVoxelValue(DataCoord(firstCoord.z, y, x));
		 fg_voxel_value = mVolume->GetVoxelValue(DataCoord(firstCoord.z, y, x));
		 
		 }
		 break;
		 */

		for(int y = firstCoord.y ; y < firstCoord.y + tl ; y++) {
			for(int x = firstCoord.x ; x < firstCoord.x + tl ; x++) {
				
				//			uint32_t bg_voxel_value = mBackgroundVolume->GetVoxelValue(DataCoord(x, y, firstCoord.z));
				//			SEGMENT_DATA_TYPE fg_voxel_value = mVolume->GetVoxelValue(DataCoord(x, y, firstCoord.z));
				
				uint32_t bg_voxel_value;
				SEGMENT_DATA_TYPE fg_voxel_value;
				
				// DOUT("datacoord = " << DataCoord(firstCoord.z, y, x));

				bg_voxel_value = mBackgroundVolume->GetVoxelValue(DataCoord(firstCoord.z, y, x));
				fg_voxel_value = mVolume->GetVoxelValue(DataCoord(firstCoord.z, y, x));
				
				//			DOUT("BG VOXEL VALUE = " << bg_voxel_value);
				//			DOUT("FG VOXEL VALUE = " << fg_voxel_value);
				
				// okay so IF fg is 0, then all bg
				// if fg != 0, then mix
				
				OmId id = current_seg.GetSegmentIdMappedToValue(fg_voxel_value);
				QColor newcolor;
				
				if(id == 0) {
					data[ctr] = bg_voxel_value;
					data[ctr+1] = bg_voxel_value;
					data[ctr+2] = bg_voxel_value;
					data[ctr+3] = 255;
				}
				else {
					
					if(current_seg.IsSegmentSelected(id)) {
						
						switch(OmStateManager::GetSystemMode()) {
							case NAVIGATION_SYSTEM_MODE: {
								newcolor = qRgba(255,255,0,255);
								
								data[ctr] = (newcolor.red() * .95) + ((bg_voxel_value) * (1.0 - .95));
								data[ctr+1] = (newcolor.green() * .95) + ((bg_voxel_value) * (1.0 - .95));
								data[ctr+2] = (newcolor.blue() * .95) + ((bg_voxel_value) * (1.0 - .95));
								data[ctr+3] = 255;
							}
								break;
								
							case EDIT_SYSTEM_MODE: {
								const Vector3<float> &color = OmVolume::GetSegmentation(myID).GetSegment(id).GetColor();
								
								newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);
								
								data[ctr] = (newcolor.red() * .95) + ((bg_voxel_value) * (1.0 - .95));
								data[ctr+1] = (newcolor.green() * .95) + ((bg_voxel_value) * (1.0 - .95));
								data[ctr+2] = (newcolor.blue() * .95) + ((bg_voxel_value) * (1.0 - .95));
								data[ctr+3] = 255;
							}
								
						}
					}
					else {
						
						const Vector3<float> &color = OmVolume::GetSegmentation(myID).GetSegment(id).GetColor();
						
						newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);
						
						data[ctr] = (newcolor.red() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
						data[ctr+1] = (newcolor.green() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
						data[ctr+2] = (newcolor.blue() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
						data[ctr+3] = 255;
					}
				}
				
				// DataCoord localDataClickPoint = DataCoord(yzDataClickPoint.x % tileLength, yzDataClickPoint.y % tileLength, 0);
				
				int xcoord = (x % tl);
				int ycoord = (y % tl);
				
				// DOUT("x, y: = " << xcoord << ", " << ycoord);
				
				//glTexSubImage2D (GL_TEXTURE_2D, 0, ycoord, xcoord, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
				
				//ctr = ctr+4;
			}	
		}
	}
	// cout << "going to make texture now" << endl;
	
	
	// glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, dims.x, dims.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void OmTile::ReplaceTextureRegion(shared_ptr<OmTextureID> &texID, int dim, set< DataCoord > &vox, QColor &color, int tl) {
	DOUT("OmTile::ReplaceTextureRegion()");
	GLuint texture = texID->GetTextureID();
	
	//glBindTexture (GL_TEXTURE_2D, texture);
	
	//	DOUT("current texture = " << texture);
	
	// so instead of relying on the color, i want to have *data be filled with the appropriate value from channel
	
	unsigned char * data = new unsigned char[4];
	
	//	data[0] = color.red();
	//	data[1] = color.green();
	//	data[2] = color.blue();
	//	data[3] = 255;
	
	set<DataCoord>::iterator itr;
	
	OmSegmentation &current_seg = OmVolume::GetSegmentation(myID);
	
	
	if(! vox.empty()) {
		
		for(itr = vox.begin(); itr != vox.end(); itr++) {
			
				// data coord is flat xy view, need to translate into other views in order to access data
			
			DataCoord vox = *itr;
			
			DataCoord orthoVox;
			switch(view_type) {
				case XY_VIEW: {
					orthoVox = DataCoord(vox.x, vox.y, vox.z);
					//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
				}
					break;
				case XZ_VIEW: {
					// ortho coord: (x, z, y) need (x, y, z)
					orthoVox = DataCoord(vox.x, vox.z, vox.y);
				}
					break;
				case YZ_VIEW: {
					// ortho coord: (z, y, x) need (x, y, z)
					orthoVox = DataCoord(vox.z, vox.x, vox.y);
				}
					break;
			}
			
			cout << "orthoVox = " << orthoVox << " ----- view = " << view_type << endl;
			// OmId id = current_seg.GetSegmentIdMappedToValue((SEGMENT_DATA_TYPE)
			uint32_t bg_voxel_value = mBackgroundVolume->GetVoxelValue(orthoVox);
			SEGMENT_DATA_TYPE fg_voxel_value = mVolume->GetVoxelValue(orthoVox);
			cout << "BG VOXEL VALUE = " << bg_voxel_value << endl;
			cout << "FG VOXEL VALUE = " << fg_voxel_value << endl;
			
			
			// okay so IF fg is 0, then all bg
			// if fg != 0, then mix
			
			OmId id = current_seg.GetSegmentIdMappedToValue(fg_voxel_value);
			QColor newcolor;
			
			if(id == 0) {
				data[0] = bg_voxel_value;
				data[1] = bg_voxel_value;
				data[2] = bg_voxel_value;
				data[3] = 255;
			}
			else {
				
				if(current_seg.IsSegmentSelected(id)) {
					
					switch(OmStateManager::GetSystemMode()) {
						case NAVIGATION_SYSTEM_MODE: {
							newcolor = qRgba(255,255,0,255);
							
							data[0] = (newcolor.red() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[1] = (newcolor.green() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[2] = (newcolor.blue() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[3] = 255;
						}
							break;
							
						case EDIT_SYSTEM_MODE: {
							const Vector3<float> &color = OmVolume::GetSegmentation(myID).GetSegment(id).GetColor();
							
							newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);
							
							data[0] = (newcolor.red() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[1] = (newcolor.green() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[2] = (newcolor.blue() * .95) + ((bg_voxel_value) * (1.0 - .95));
							data[3] = 255;
						}
							
					}
				}
				else {
					
					const Vector3<float> &color = OmVolume::GetSegmentation(myID).GetSegment(id).GetColor();
					
					newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);
					
					data[0] = (newcolor.red() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
					data[1] = (newcolor.green() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
					data[2] = (newcolor.blue() * mAlpha) + ((bg_voxel_value) * (1.0 - mAlpha));
					data[3] = 255;
				}
			}
			
			// DataCoord localDataClickPoint = DataCoord(yzDataClickPoint.x % tileLength, yzDataClickPoint.y % tileLength, 0);
			
			int xcoord = ((*itr).x % tl);
			int ycoord = ((*itr).y % tl);
			
			DOUT("x, y: = " << xcoord << ", " << ycoord);
			
			//glTexSubImage2D (GL_TEXTURE_2D, 0, xcoord, ycoord, dim, dim, GL_RGBA, GL_UNSIGNED_BYTE, data);
			
		}
	}
	
}
