#ifndef OM_MESH_DRAWER_H
#define OM_MESH_DRAWER_H

#include <QExplicitlySharedDataPointer>

#include "segment/omSegmentPointers.h"
#include "volume/omSegmentation.h"

class OmSegmentIterator;
class OmViewGroupState;
class OmMipChunkCoord;
class OmMipChunk;
class OmSegmentation;
class OmVolumeCuller;

class OmMeshDrawer : boost::noncopyable
{
 public:
	OmMeshDrawer(const OmId, OmViewGroupState *);
	~OmMeshDrawer();

	//drawing
	void Draw(OmVolumeCuller &);

 private:

	OmSegmentation * mSeg;
	OmSegmentCache * mSegmentCache;
	OmViewGroupState * mViewGroupState;
	OmVolumeCuller * mVolumeCuller;

	bool mIterOverSelectedIDs;
	bool mIterOverEnabledIDs;

	void DrawChunkRecursive(const OmMipChunkCoord &, bool testVis );

	void DrawChunk(QExplicitlySharedDataPointer < OmMipChunk > p_chunk,
		       const OmMipChunkCoord & chunkCoord);

	void DrawMeshes(const OmBitfield & drawOps,
			const OmMipChunkCoord & mipCoord, 
			const OmSegPtrs  & segmentsToDraw );

	bool DrawCheck(QExplicitlySharedDataPointer < OmMipChunk > p_chunk);
	
	void populateSegIDsCache(QExplicitlySharedDataPointer < OmMipChunk > p_chunk,
				 const OmMipChunkCoord & chunkCoord);
};

#endif
