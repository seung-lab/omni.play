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

class OmMeshDrawer 
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

	void DrawChunkRecursive(const OmMipChunkCoord &, 
				OmSegmentIterator iter,
				bool testVis );

	void DrawChunk(QExplicitlySharedDataPointer < OmMipChunk > p_chunk,
		       const OmMipChunkCoord & chunkCoord,
		       const OmSegPtrs & segmentsToDraw);

	void DrawMeshes(const OmBitfield & drawOps,
			const OmMipChunkCoord & mipCoord, 
			const OmSegPtrs  & segmentsToDraw );

};

#endif
