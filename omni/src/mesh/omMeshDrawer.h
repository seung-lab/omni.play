#ifndef OM_MESH_DRAWER_H
#define OM_MESH_DRAWER_H

#include "volume/omMipVolume.h"
#include "segment/omSegmentPointers.h"

class OmSegment;
class OmMipChunkCoord;
class OmSegmentIterator;
class OmSegmentation;
class OmViewGroupState;
class OmVolumeCuller;

class OmMeshDrawer : boost::noncopyable
{
public:
	OmMeshDrawer(OmSegmentation*, OmViewGroupState*);
	virtual ~OmMeshDrawer(){}

	void Draw(OmVolumeCuller &);

private:
	OmSegmentation *const mSeg;
	OmViewGroupState *const mViewGroupState;
	const boost::shared_ptr<OmSegmentCache> mSegmentCache;

	boost::shared_ptr<OmVolumeCuller> mVolumeCuller;
	OmSegPtrList mRootSegsToDraw;

	void DrawChunkRecursive(const OmMipChunkCoord &, bool testVis );
	void DrawChunk(OmMipChunkPtr);
	void doDrawChunk(const OmMipChunkCoord &, const OmSegPtrList &);
	bool ShouldChunkBeDrawn(OmMipChunkPtr p_chunk);
	void DrawClippedExtent(OmMipChunkPtr p_chunk);
	void ColorMesh(const OmBitfield & drawOps, OmSegment * segment);
	void ApplyColor(OmSegment * seg, const OmBitfield & drawOps,
					const OmSegmentColorCacheType sccType);
};

#endif
