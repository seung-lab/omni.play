#pragma once

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
    boost::optional<std::string> IsEdgeSplittable(const OmSegmentEdge& e);
    boost::optional<std::string> IsSegmentSplittable(OmSegment* child);

    void refreshTree();

    bool AreAnySegmentsInValidList(const OmSegIDsSet& ids);

private:
    OmUserEdges* userEdges_;

    OmSegmentEdge splitChildFromParentNoTest(OmSegment* child);

    std::pair<bool, OmSegmentEdge> JoinEdgeFromUser(const OmSegmentEdge& e);
    std::pair<bool, OmSegmentEdge> JoinFromUserAction(const OmID, const OmID);

    void rerootSegmentLists();
    void setGlobalThreshold(OmMST* mst);
    void resetGlobalThreshold(OmMST* mst);

    friend class OmSegmentColorizer;
    friend QDataStream& operator<<(QDataStream&, const OmSegmentsImpl&);
    friend QDataStream& operator>>(QDataStream&, OmSegmentsImpl&);
};

