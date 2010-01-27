#ifndef OM_TEXTURE_ID_H
#define OM_TEXTURE_ID_H

/*
 *	omTextureIDs are a wrapper for GLuint texture IDs.  They associate a texture ID with the corresponding TileCoord,
 *  and keep track of the size of the texture in memory.
 *	
 *	A TextureID is associated with a TextureIDCache that keeps track of how much GL video memory is in use.
 *
 *	Rachel Shearer - rshearer@mit.edu - 3/17/09
 */

#include "common/omStd.h"
#include "common/omGl.h"
#include "system/omSystemTypes.h"
#include "volume/omVolumeTypes.h"
//#include "system/omCacheableObject.h"
#include "system/omCacheableBase.h"

#include "system/omGenericManager.h"

#include "omTileCoord.h"

// Flags to OmTextureID. FIXME. All of this is just a hack to make it work for now. MW.
#define OMTILE_FIXME		0
#define OMTILE_NEEDTEXTUREBUILT 1
#define OMTILE_COORDINVALID     2
#define OMTILE_GOOD		3
#define OMTILE_NEEDCOLORMAP 	4

#include <boost/tuple/tuple_comparison.hpp>
using boost::tuple;

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;

class OmThreadedCachingTile;

// typedef tuple<int, SpaceCoord> OmTileCoordinate;     

class OmTextureID;
//class OmTileCoord;

class OmTextureID:protected OmCacheableBase {

 public:
	OmTextureID(const OmTileCoord & tileCoord, const GLuint & texID, const int &size, const int x, const int y,
		    const OmIds & containedIds, OmThreadedCachingTile * cache = NULL, void *texture = NULL, int flags =
		    0);
	~OmTextureID();

	//texture ID property accessors;
	const GLuint GetTextureID() const {
		return textureID;
	} const int GetSize() const {
		return mem_size;
	} const OmTileCoord & GetCoordinate() const {
		return mTileCoordinate;
	} const OmIds GetIds() const {
		return mIdSet;
	} const bool FindId(OmId f_id);

	class OmTexureIDSafeDelete:public QObject {
 Q_OBJECT public:
		OmTexureIDSafeDelete(GLuint textureID);
		~OmTexureIDSafeDelete();
 private:
		GLuint mTextureID;
	};

	GLuint textureID;
	OmTileCoord mTileCoordinate;
	ObjectType mVolType;
	bool mRemoveMe;
	OmThreadedCachingTile *mCache;

	OmIds mIdSet;

	int flags;
	int x;
	int y;
	void *texture;		// Free this once the texture is built.

	//texture properties    
	int mem_size;		// total size of data in memory: width * height * bytesPerSample * samplesPerVoxel

	friend ostream & operator<<(ostream & out, const OmTextureID & tid);

	friend class myQGLWidget;
	friend class OmView2d;
	friend class DOmView2d;
 private:

};

#endif
