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
	OmSegmentCache *const mSegmentCache;

	boost::shared_ptr<OmVolumeCuller> mVolumeCuller;
	OmSegPtrList mRootSegsToDraw;
	bool redrawNeeded_;

	void drawChunkRecursive(const OmMipChunkCoord &, bool testVis );
	void drawChunk(OmMipChunkPtr);
	void doDrawChunk(const OmMipChunkCoord &, const OmSegPtrList &);
	bool shouldChunkBeDrawn(OmMipChunkPtr p_chunk);
	void drawClippedExtent(OmMipChunkPtr p_chunk);
	void colorMesh(const OmBitfield & drawOps, OmSegment * segment);
	void applyColor(OmSegment * seg, const OmBitfield & drawOps,
					const OmSegmentColorCacheType sccType);
};

#endif
