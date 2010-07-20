#ifndef OM_MESH_DRAWER_H
#define OM_MESH_DRAWER_H

#include "segment/omSegmentPointers.h"
#include "volume/omMipChunkPtr.h"

class OmMipChunkCoord;
class OmSegmentIterator;
class OmSegmentation;
class OmViewGroupState;
class OmVolumeCuller;

class OmMeshDrawer : boost::noncopyable
{
 public:
	OmMeshDrawer(const OmId, OmViewGroupState *);
	~OmMeshDrawer();
	void Init();

	void Draw(OmVolumeCuller &);

 private:
	const OmId mSegmentationID;
	OmSegmentation * mSeg;
	OmSegmentCache * mSegmentCache;
	OmViewGroupState * mViewGroupState;
	OmVolumeCuller * mVolumeCuller;

	bool mIterOverSelectedIDs;
	bool mIterOverEnabledIDs;

	void checkCache();

	void addToCache( const OmMipChunkCoord &, const OmSegPtrList & );
	bool cacheHasCoord( const OmMipChunkCoord &);
	const OmSegPtrList & getFromCache( const OmMipChunkCoord & );
	void makeSegmentListForCache(OmMipChunkPtr p_chunk, const OmMipChunkCoord & chunkCoord);

	void DrawChunkRecursive(const OmMipChunkCoord &, bool testVis );
	void DrawChunk(OmMipChunkPtr p_chunk, const OmMipChunkCoord & chunkCoord);
	bool ShouldChunkBeDrawn(OmMipChunkPtr p_chunk);
	void DrawClippedExtent(OmMipChunkPtr p_chunk);
	void ColorMesh(const OmBitfield & drawOps, OmSegment * segment);
	void ApplyColor(OmSegment * seg, const OmBitfield & drawOps, 
			const OmSegmentColorCacheType sccType);
};

#endif
