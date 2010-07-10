#ifndef OM_MESH_SEGMENT_LIST_H
#define OM_MESH_SEGMENT_LIST_H

#include "common/omCommon.h"
#include "volume/omMipChunkPtr.h"
#include "volume/omMipChunkCoord.h"
#include "segment/omSegmentPointers.h"

#include <QThread>
#include <QMutex>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
typedef boost::tuple<OmId,OmSegID,int,int,int,int> OmMeshSegListKey;

class OmSegment;
class OmSegmentCache;
class OmMeshSegmentListThread;

class OmMeshSegmentList : boost::noncopyable{
 public:
	static void Delete();

	static const OmSegPtrList & getFromCache( const OmMipChunkCoord &, 
						  const OmSegID,
						  const OmId);
	
	static bool isSegmentListReadyInCache(OmMipChunkPtr, 
					      OmSegment *,
					      const OmMipChunkCoord &,
					      OmSegmentCache *,
					      const OmId );

	static void addToCache( const OmMipChunkCoord &, 
				OmSegment *, 
				const OmSegPtrList &,
				const OmId );
 private:
	OmMeshSegmentList();
	~OmMeshSegmentList();
	static OmMeshSegmentList* Instance();

	//singleton
	static OmMeshSegmentList* mspInstance;

	std::map<OmMeshSegListKey, OmSegPtrListValid> mSegmentListCache;
	
	QMutex mCacheLock;

	bool doIsSegmentListReadyInCache(OmMipChunkPtr, 
					 OmSegment *,
					 const OmMipChunkCoord &,
					 OmSegmentCache *,
					 const OmId );
	
	bool doCacheHasCoord( const OmMipChunkCoord &, 
			      const OmSegID,
			      const OmId );

	const OmSegPtrList & doGetFromCache( const OmMipChunkCoord &, 
					     const OmSegID,
					     const OmId );

	void doMakeSegmentListForCache(OmMipChunkPtr, OmSegment *, 
				       const OmMipChunkCoord &,
				       OmSegmentCache *,
				       const OmId  );

	void doClearFromCacheIfFreshnessInvalid( const OmMipChunkCoord & c,
						 OmSegment * rootSeg,
						 const OmId );

	bool doIsCacheFetching( const OmMipChunkCoord & c,
				OmSegment * rootSeg,
				const OmId  );
	
	void doLetCacheKnowWeAreFetching( const OmMipChunkCoord & c,
					  OmSegment * rootSeg,
					  const OmId  );

	void doAddToCache( const OmMipChunkCoord &, 
			   OmSegment *, 
			   const OmSegPtrList &,
			   const OmId  );
};

#endif
