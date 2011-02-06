#ifndef OM_SEGMENT_CACHE_IMPL_H
#define OM_SEGMENT_CACHE_IMPL_H

#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"

class OmChunkCoord;
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

    std::pair<bool, OmSegmentEdge> JoinFromUserAction(const OmSegmentEdge& e);
    OmSegmentEdge SplitEdgeUserAction(const OmSegmentEdge& e);
    OmSegIDsSet JoinTheseSegments(const OmSegIDsSet& segmentList);
    OmSegIDsSet UnJoinTheseSegments(const OmSegIDsSet& segmentList);

    void refreshTree();
    quint64 getSizeRootAndAllChildren(OmSegment* segUnknownDepth);

    bool AreAnySegmentsInValidList(const OmSegIDsSet& ids);

private:
    OmUserEdges* userEdges_;

    OmSegmentEdge splitChildFromParent(OmSegment* child);

    std::pair<bool, OmSegmentEdge> JoinEdgeFromUser(const OmSegmentEdge& e);
    std::pair<bool, OmSegmentEdge> JoinFromUserAction(const OmID, const OmID);

    void rerootSegmentLists();
    void setGlobalThreshold();
    void resetGlobalThreshold();

    friend class OmSegmentColorizer;
    friend QDataStream& operator<<(QDataStream&, const OmSegmentCacheImpl&);
    friend QDataStream& operator>>(QDataStream&, OmSegmentCacheImpl&);
};

#endif
