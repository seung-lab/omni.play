#ifndef OM_SEGMENT_CACHE_IMPL_H
#define OM_SEGMENT_CACHE_IMPL_H

#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "segment/omSegmentPointers.h"

#include <QList>

class OmMipChunkCoord;
class OmSegmentEdge;

class OmSegmentCacheImpl : public OmSegmentCacheImplLowLevel {
 public:
	OmSegmentCacheImpl( OmSegmentation *, OmSegmentCache *);
	~OmSegmentCacheImpl();

	OmSegment* AddSegment();
	OmSegment* AddSegment(OmSegID value);
	void AddSegmentsFromChunk(const OmSegIDsSet &, const OmMipChunkCoord &,
				  boost::unordered_map< OmSegID, unsigned int> * sizes );

	OmSegmentEdge * splitTwoChildren(OmSegment * seg1, OmSegment * seg2);
	OmSegmentEdge * splitChildFromParent( OmSegment * child );

	OmSegmentEdge * JoinFromUserAction( OmSegmentEdge * e );
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
	void loadDendrogram();
	OmSegmentEdge * JoinEdge( OmSegmentEdge * e );
	QList<OmSegmentEdge*> mManualUserMergeEdgeList;

	OmSegmentEdge * JoinFromUserAction( const OmId, const OmId );

	void setSegmentSelectedBatch( OmSegID segID, bool isSelected );

	void updateSizeListsFromSplit( OmSegment * parent, OmSegment * child );
	void updateSizeListsFromJoin( OmSegment * root, OmSegment * child );
	void doSelectedSetInsert( const OmSegID segID);
	void doSelectedSetRemove( const OmSegID segID);
	quint64 getRecentActivity();
	void addToRecentMap( const OmSegID segID);
	
	friend class OmSegmentColorizer;
	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCacheImpl & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCacheImpl & sc );
};

#endif
