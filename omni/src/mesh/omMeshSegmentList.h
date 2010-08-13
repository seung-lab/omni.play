#ifndef OM_MESH_SEGMENT_LIST_H
#define OM_MESH_SEGMENT_LIST_H

#include "common/omCommon.h"
#include "volume/omMipChunkCoord.h"
#include "segment/omSegmentPointers.h"
#include "volume/omMipVolume.h"

#include <zi/mutex>
#include "utility/OmThreadPool.hpp"

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

  static inline std::pair<bool, OmSegPtrList>
  getFromCacheIfReady(OmMipChunkPtr p_chunk,
		      OmSegment * rootSeg,
		      const OmMipChunkCoord & chunkCoord,
		      boost::shared_ptr<OmSegmentCache> mSegmentCache,
		      const OmId segmentationID)
  {
    return Instance()->doGetFromCacheIfReady(p_chunk,
					     rootSeg,
					     chunkCoord,
					     mSegmentCache,
					     segmentationID);
  }

  static void addToCache( const OmMipChunkCoord &,
			  OmSegment *,
			  const OmSegPtrList &,
			  const OmId );

private:
  //singleton
  OmMeshSegmentList();
  ~OmMeshSegmentList();
  static OmMeshSegmentList* Instance();
  static OmMeshSegmentList* mspInstance;

  std::map<OmMeshSegListKey, OmSegPtrListValid> mSegmentListCache;
  OmThreadPool mThreadPool;
  zi::Mutex mutex;

  std::pair<bool, OmSegPtrList>
  doGetFromCacheIfReady(OmMipChunkPtr,
		      OmSegment *,
		      const OmMipChunkCoord &,
		      boost::shared_ptr<OmSegmentCache>,
		      const OmId );

  static inline OmMeshSegListKey makeKey(const OmId segmentationID,
					 const OmSegID segID,
					 const OmMipChunkCoord & c){
    return OmMeshSegListKey(segmentationID,
			    segID,
			    c.Level,
			    c.Coordinate.x,
			    c.Coordinate.y,
			    c.Coordinate.z);
  }

};

#endif
