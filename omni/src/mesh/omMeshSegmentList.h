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

	static bool cacheHasCoord( const OmMipChunkCoord &, const OmSegID,
				   const OmId mSegmentationID);

	static const OmSegPtrList & getFromCache( const OmMipChunkCoord &, 
						  const OmSegID,
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

	static void letCacheKnowWeAreFetching( const OmMipChunkCoord & c,
					       OmSegment * rootSeg,
					       const OmId mSegmentationID );

	static void addToCache( const OmMipChunkCoord &, OmSegment *, const OmSegPtrList &,
				const OmId mSegmentationID );


 private:
	OmMeshSegmentList();
	~OmMeshSegmentList();
	static OmMeshSegmentList* Instance();

	//singleton
	static OmMeshSegmentList* mspInstance;

	std::map<OmMeshSegListKey, OmSegPtrListValid> mSegmentListCache;
	
	QMutex mCacheLock;

	bool doCacheHasCoord( const OmMipChunkCoord &, const OmSegID,
			      const OmId mSegmentationID);

	const OmSegPtrList & doGetFromCache( const OmMipChunkCoord &, 
					     const OmSegID,
					     const OmId mSegmentationID);

	void doMakeSegmentListForCache(OmMipChunkPtr, OmSegment *, const OmMipChunkCoord &,
				       OmSegmentCache *,
				       const OmId mSegmentationID );

	void doClearFromCacheIfFreshnessInvalid( const OmMipChunkCoord & c,
						 OmSegment * rootSeg,
						 const OmId mSegmentationID);

	bool doIsCacheFetching( const OmMipChunkCoord & c,
				OmSegment * rootSeg,
				const OmId mSegmentationID );
	
	void doLetCacheKnowWeAreFetching( const OmMipChunkCoord & c,
					  OmSegment * rootSeg,
					  const OmId mSegmentationID );

	void doAddToCache( const OmMipChunkCoord &, OmSegment *, const OmSegPtrList &,
			   const OmId mSegmentationID );
};

#endif
