#ifndef OM_TEXTURE_ID_H
#define OM_TEXTURE_ID_H

/*
 *      omTextureIDs are a wrapper for GLuint texture IDs.  They associate a texture ID with
 *      the corresponding TileCoord, and keep track of the size of the texture in memory.
 *	
 *	A TextureID is associated with a TextureIDCache that keeps track of how much GL video memory is in use.
 *
 *	Rachel Shearer - rshearer@mit.edu - 3/17/09
 */

#include "common/omStd.h"
#include "common/omGl.h"
#include "system/omCacheableBase.h"
#include "system/omGenericManager.h"
#include "omTileCoord.h"

// Flags to OmTextureID. FIXME. All of this is just a hack to make it work for now. MW.
#define OMTILE_FIXME		0
#define OMTILE_NEEDTEXTUREBUILT 1
#define OMTILE_COORDINVALID     2
#define OMTILE_GOOD		3
#define OMTILE_NEEDCOLORMAP 	4

class OmThreadedCachingTile;

class OmTextureID;

class OmTextureID : public OmCacheableBase {
	
public:
	OmTextureID(const OmTileCoord &tileCoord, 
		    const GLuint &texID, 
		    const int &size, 
		    const int x, 
		    const int y, 
		    OmThreadedCachingTile *cache = NULL, 
		    void* texture = NULL, 
		    int flags = 0);
	~OmTextureID();
	
	//texture ID property accessors;
	GLuint GetTextureID() const { return textureID; }
	int GetSize() const { return mem_size; }
	const OmTileCoord& GetCoordinate() const { return mTileCoordinate; }
		
	void Flush();

	OmTileCoord mTileCoordinate;
	GLuint textureID;
	int mem_size;	// total size of data in memory: width * height * bytesPerSample * samplesPerVoxel
	void * texture;	// Free this once the texture is built.
	int flags;
	int x;
	int y;

	ObjectType mVolType;
	bool mRemoveMe;
	OmThreadedCachingTile *mCache;
		
	friend ostream& operator<<(ostream &out, const OmTextureID &tid);

	friend class myQGLWidget;
	friend class OmView2d;
	friend class DOmView2d;

};

#endif
