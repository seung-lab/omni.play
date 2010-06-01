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
	OmMeshDrawer(OmSegmentation * seg);

	//drawing
	void Draw(OmVolumeCuller &, OmViewGroupState * vgs);

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
};

#endif
