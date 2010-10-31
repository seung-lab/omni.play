#ifndef OM_SEGMENT_CACHE_IMPL_H
#define OM_SEGMENT_CACHE_IMPL_H

#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "segment/omSegmentPointers.h"
#include "volume/omVolumeTypes.hpp"

#include <QList>

class OmMipChunkCoord;
class OmSegmentEdge;
class OmUserEdges;

class OmSegmentCacheImpl : public OmSegmentCacheImplLowLevel {
public:
	OmSegmentCacheImpl(OmSegmentation*);
	virtual ~OmSegmentCacheImpl();

	void Flush();

	OmSegment* AddSegment();
	OmSegment* AddSegment(OmSegID value);
	OmSegment* GetOrAddSegment(const OmSegID val);

	OmSegmentEdge findClosestCommonEdge(OmSegment*, OmSegment*);

	std::pair<bool, OmSegmentEdge> JoinFromUserAction(OmSegmentEdge e);
	OmSegmentEdge SplitEdgeUserAction(OmSegmentEdge e);
	void JoinTheseSegments(const OmSegIDsSet& segmentList);
	void UnJoinTheseSegments(const OmSegIDsSet& segmentList);

	void refreshTree();
	quint64 getSizeRootAndAllChildren(OmSegment* segUnknownDepth);

private:
	OmSegmentEdge splitChildFromParent(OmSegment* child);

	boost::shared_ptr<OmUserEdges> userEdges();

	std::pair<bool, OmSegmentEdge> JoinEdgeFromUser(OmSegmentEdge e);
	std::pair<bool, OmSegmentEdge> JoinFromUserAction(const OmID, const OmID);

	void rerootSegmentLists();
	void rerootSegmentList(OmSegIDsSet& set);
	void setGlobalThreshold();
	void resetGlobalThreshold();

	inline boost::shared_ptr<OmSegmentLists> getSegmentLists();

	friend class OmSegmentColorizer;
	friend QDataStream& operator<<(QDataStream&, const OmSegmentCacheImpl&);
	friend QDataStream& operator>>(QDataStream&, OmSegmentCacheImpl&);
};

#endif
