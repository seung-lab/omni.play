#ifndef OM_MESH_SEGMENT_LIST_H
#define OM_MESH_SEGMENT_LIST_H

#include "common/omCommon.h"
#include "volume/omMipChunkCoord.h"
#include "segment/omSegmentPointers.h"
#include "volume/omMipVolume.h"

#include "zi/omMutex.h"
#include "utility/omThreadPool.hpp"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
typedef boost::tuple<OmId,OmSegID,int,int,int,int> OmMeshSegListKey;

class OmSegment;
class OmSegmentCache;
class OmMeshSegmentListThread;

class OmSegPtrListValid {
public:
	OmSegPtrListValid()
		: isValid(false)
		, freshness(0)
		, isFetching(false)
	{}
	explicit OmSegPtrListValid(const bool isFetching)
		: isValid(false)
		, freshness(0)
		, isFetching(isFetching)
	{}
	OmSegPtrListValid( const OmSegPtrList & L, const quint32 f )
		: isValid(true)
		, list(L)
		, freshness(f)
		, isFetching(false)
	{}

	bool isValid;
	OmSegPtrList list;
	quint32 freshness;
	bool isFetching;
};

class OmMeshSegmentList : boost::noncopyable{
public:
	static void Delete();

	static inline boost::optional<OmSegPtrList>
	getFromCacheIfReady(OmMipChunkPtr chunk, OmSegment* rootSeg) {
		return Instance()->doGetFromCacheIfReady(chunk, rootSeg);
	}

	static void addToCache(OmMipChunkPtr, OmSegment*,
						   const OmSegPtrList &);

private:
	//singleton
	OmMeshSegmentList();
	~OmMeshSegmentList();
	static OmMeshSegmentList* Instance();
	static OmMeshSegmentList* mspInstance;

	std::map<OmMeshSegListKey, OmSegPtrListValid> mSegmentListCache;
	OmThreadPool mThreadPool;
	zi::mutex mutex;

	boost::optional<OmSegPtrList> doGetFromCacheIfReady(OmMipChunkPtr, OmSegment*);

	static OmMeshSegListKey makeKey(OmMipChunkPtr chunk, OmSegment* rootSeg);
};

#endif
