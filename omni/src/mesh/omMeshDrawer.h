#ifndef OM_MESH_DRAWER_H
#define OM_MESH_DRAWER_H

#include <QExplicitlySharedDataPointer>
#include "segment/omSegmentPointers.h"
#include "volume/omSegmentation.h"

class OmSegmentIterator;
class OmViewGroupState;
class OmVolumeCuller;
class OmMipChunkCoord;
class OmMipChunk;
class OmSegmentation;

class OmMeshDrawer 
{
 public:
	OmMeshDrawer(const OmId, OmViewGroupState *);

	//drawing
	void Draw(OmVolumeCuller &);

 private:

	OmSegmentation * mSeg;
	OmSegmentCache * mSegmentCache;
	OmViewGroupState * mViewGroupState;

	void DrawChunkRecursive(const OmMipChunkCoord &, 
				OmSegmentIterator iter,
				bool testVis, 
				OmVolumeCuller &);

	void DrawChunk(QExplicitlySharedDataPointer < OmMipChunk > p_chunk,
		       const OmMipChunkCoord & chunkCoord,
		       const OmSegPtrs & segmentsToDraw, 
		       OmVolumeCuller &rCuller);

	void DrawMeshes(const OmBitfield & drawOps,
			const OmMipChunkCoord & mipCoord, 
			const OmSegPtrs  & segmentsToDraw );

};

#endif
