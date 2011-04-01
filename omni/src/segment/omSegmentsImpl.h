#ifndef OM_SEGMENT_CACHE_IMPL_H
#define OM_SEGMENT_CACHE_IMPL_H

#include "segment/lowLevel/omSegmentsImplLowLevel.h"

class OmChunkCoord;
class OmSegmentEdge;
class OmUserEdges;
class OmSegmentChildren;
class OmSegmentsStore;

class OmSegmentsImpl : public OmSegmentsImplLowLevel {
public:
    OmSegmentsImpl(OmSegmentation*, OmSegmentsStore*);
    virtual ~OmSegmentsImpl();

    void Flush();

    OmSegment* AddSegment();
    OmSegment* AddSegment(OmSegID value);
    OmSegment* GetOrAddSegment(const OmSegID val);

    OmSegmentChildren* Children();

    std::pair<bool, OmSegmentEdge> JoinFromUserAction(const OmSegmentEdge& e);
    OmSegmentEdge SplitEdgeUserAction(const OmSegmentEdge& e);
    OmSegIDsSet JoinTheseSegments(const OmSegIDsSet& segmentList);
    OmSegIDsSet UnJoinTheseSegments(const OmSegIDsSet& segmentList);

    void refreshTree();

    bool AreAnySegmentsInValidList(const OmSegIDsSet& ids);

private:
    OmUserEdges* userEdges_;

    OmSegmentEdge splitChildFromParent(OmSegment* child);

    std::pair<bool, OmSegmentEdge> JoinEdgeFromUser(const OmSegmentEdge& e);
    std::pair<bool, OmSegmentEdge> JoinFromUserAction(const OmID, const OmID);

    void rerootSegmentLists();
    void setGlobalThreshold(OmMST* mst);
    void resetGlobalThreshold(OmMST* mst);

    friend class OmSegmentColorizer;
    friend QDataStream& operator<<(QDataStream&, const OmSegmentsImpl&);
    friend QDataStream& operator>>(QDataStream&, OmSegmentsImpl&);
};

#endif
