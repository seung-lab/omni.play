#ifndef OM_SEGMENT_CACHE_IMPL_H
#define OM_SEGMENT_CACHE_IMPL_H

#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "segment/omSegmentPointers.h"
#include "volume/omVolumeTypes.hpp"

#include <QList>

class OmMipChunkCoord;
class OmSegmentEdge;

class OmSegmentCacheImpl : public OmSegmentCacheImplLowLevel {
public:
	OmSegmentCacheImpl(OmSegmentation *);
	virtual ~OmSegmentCacheImpl();

	void Flush();

	OmSegment* AddSegment();
	OmSegment* AddSegment(OmSegID value);
	OmSegment* GetOrAddSegment(const OmSegID val);

	OmSegmentEdge findClosestCommonEdge(OmSegment *, OmSegment *);

	std::pair<bool, OmSegmentEdge> JoinFromUserAction( OmSegmentEdge e );
	OmSegmentEdge SplitEdgeUserAction( OmSegmentEdge e );
	void JoinTheseSegments( const OmSegIDsSet & segmentList);
	void UnJoinTheseSegments( const OmSegIDsSet & segmentList);

	OmSegPtrListWithPage * getRootLevelSegIDs( const unsigned int offset,
											   const int numToGet,
											   const OmSegIDRootType type,
											   const OmSegID starSeg = 0);

	void setAsValidated(OmSegment * segment, const bool valid);

	void refreshTree();
	quint64 getSizeRootAndAllChildren( OmSegment * segUnknownDepth );

	quint64 getSegmentListSize(OmSegIDRootType type);

private:
	OmSegmentEdge splitChildFromParent( OmSegment * child );

	QList<OmSegmentEdge> mManualUserMergeEdgeList;

	std::pair<bool, OmSegmentEdge> JoinEdgeFromUser( OmSegmentEdge e );
	std::pair<bool, OmSegmentEdge> JoinFromUserAction( const OmID, const OmID );

	void rerootSegmentLists();
	void rerootSegmentList( OmSegIDsSet & set );
	void setGlobalThreshold();
	void resetGlobalThreshold();

	inline boost::shared_ptr<OmSegmentLists> getSegmentLists();

	friend class OmSegmentColorizer;
	friend QDataStream &operator<<(QDataStream&, const OmSegmentCacheImpl&);
	friend QDataStream &operator>>(QDataStream&, OmSegmentCacheImpl&);
};

#endif
