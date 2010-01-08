#ifndef OM_TILE_H
#define OM_TILE_H


/*
 *	OmTile allows access to 2D image data from the source volume.
 *
 *	Rachel Shearer - rshearer@mit.edu
 */


#include "common/omStd.h"

#include "volume/omMipChunkCoord.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;

#include "segment/omSegmentTypes.h"

#include "system/omSystemTypes.h"
#include "volume/omVolumeTypes.h"

#include <QColor>

#include <boost/tuple/tuple_comparison.hpp>
using boost::tuple;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

class OmMipVolume;
class OmTextureID;
class OmTileCoord;

class OmTile {

public:
	OmTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol);
	~OmTile();
	
	OmTextureID* BindToTextureID(const OmTileCoord &key);
	// int BindToTextureID(const OmTileCoordinate &key);
	
	void AddOverlay(ObjectType secondtype, OmId second_id, OmMipVolume *secondvol);
	void SetNewAlpha(float newval);

	OmMipVolume *mVolume;

	
// private:	
	void* GetImageData(const OmTileCoord &key, Vector2<int> &sliceDims, OmMipVolume *vol);
	OmMipChunkCoord TileToMipCoord(const OmTileCoord &key);
	int GetDepth(const OmTileCoord &key, OmMipVolume *vol);
	void setMergeChannels(unsigned char *imageData, unsigned char *secondImageData, Vector2<int> dims, Vector2<int> second_dims, const OmTileCoord &key);
	OmIds setMyColorMap(SEGMENT_DATA_TYPE* imageData, Vector2<int> dims, const OmTileCoord &key, void **rData);
	OmIds setMyColorMap(SEGMENT_DATA_TYPE* imageData, unsigned char* secondImageData, Vector2<int> dims, Vector2<int> second_dims, const OmTileCoord &key);

	void ReplaceFullTextureRegion(shared_ptr<OmTextureID> &texID, DataCoord firstCoord, int tl);
	void ReplaceTextureRegion(shared_ptr<OmTextureID> &texID, int dim, set< DataCoord > &vox, QColor &color, int tl);
private:
	
	ViewType view_type;
	ObjectType vol_type;
	OmId myID;

	
	ObjectType background_type;
	OmId backgroundID;
	OmMipVolume *mBackgroundVolume;
	
	float mAlpha;
	int mSamplesPerVoxel;
	int mBytesPerSample;
	
	int mBackgroundSamplesPerVoxel;
	int mBackgroundBytesPerSample;
};

#endif
