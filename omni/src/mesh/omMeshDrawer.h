#ifndef OM_MESH_DRAWER_H
#define OM_MESH_DRAWER_H

#include "segment/omSegmentPointers.h"
#include "volume/omMipChunkPtr.h"

class OmSegment;
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

	static void letCacheKnowWeAreFetching( const OmMipChunkCoord & c,
					       OmSegment * rootSeg,
					       const OmId mSegmentationID );
	static void addToCache( const OmMipChunkCoord &, OmSegment *, const OmSegPtrList &,
				const OmId mSegmentationID );

 private:
	const OmId mSegmentationID;
	OmSegmentation * mSeg;
	OmSegmentCache * mSegmentCache;
	OmViewGroupState * mViewGroupState;
	OmVolumeCuller * mVolumeCuller;

	OmSegPtrList mRootSegsToDraw;

	static bool cacheHasCoord( const OmMipChunkCoord &, const OmSegID,
				   const OmId mSegmentationID);
	static const OmSegPtrList & getFromCache( const OmMipChunkCoord &, const OmSegID,
						  const OmId mSegmentationID);
	static void makeSegmentListForCache(OmMipChunkPtr, OmSegment *, const OmMipChunkCoord &,
					    OmSegmentCache *,
					    const OmId mSegmentationID );
	static void clearFromCacheIfFreshnessInvalid( const OmMipChunkCoord & c,
						      OmSegment * rootSeg,
						      const OmId mSegmentationID);
	static bool isCacheFetching( const OmMipChunkCoord & c,
				     OmSegment * rootSeg,
				     const OmId mSegmentationID );

	void DrawChunkRecursive(const OmMipChunkCoord &, bool testVis );
	void DrawChunk(OmMipChunkPtr p_chunk, const OmMipChunkCoord & chunkCoord);
	void doDrawChunk(const OmMipChunkCoord &, const OmSegPtrList &);
	bool ShouldChunkBeDrawn(OmMipChunkPtr p_chunk);
	void DrawClippedExtent(OmMipChunkPtr p_chunk);
	void ColorMesh(const OmBitfield & drawOps, OmSegment * segment);
	void ApplyColor(OmSegment * seg, const OmBitfield & drawOps, 
			const OmSegmentColorCacheType sccType);
};

#endif
