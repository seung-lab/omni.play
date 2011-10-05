#pragma once

#include "segment/lowLevel/segmentsImplLowLevel.h"
#include "datalayer/archive/segmentation.h"

class OmSegmentEdge;
class OmUserEdges;
class OmSegmentChildren;
class segmentsStore;

class segmentsImpl : public segmentsImplLowLevel {
public:
    segmentsImpl(segmentation*, segmentsStore*);
    virtual ~segmentsImpl();

    void Flush();

    OmSegment* AddSegment();
    OmSegment* AddSegment(segId value);
    OmSegment* GetOrAddSegment(const segId val);

    OmSegmentChildren* Children();

    std::pair<bool, OmSegmentEdge> JoinFromUserAction(const OmSegmentEdge& e);
    OmSegmentEdge SplitEdgeUserAction(const OmSegmentEdge& e);
    segIdsSet JoinTheseSegments(const segIdsSet& segmentList);
    segIdsSet UnJoinTheseSegments(const segIdsSet& segmentList);
    boost::optional<std::string> IsEdgeSplittable(const OmSegmentEdge& e);
    boost::optional<std::string> IsSegmentSplittable(OmSegment* child);
	boost::optional<std::string> IsSegmentCuttable(OmSegment* seg);

    void refreshTree();

    bool AreAnySegmentsInValidList(const segIdsSet& ids);

	std::vector<OmSegmentEdge> CutSegment(OmSegment* seg);
	bool JoinEdges(const std::vector<OmSegmentEdge>& edges);

private:
    OmUserEdges* userEdges_;

    OmSegmentEdge splitChildFromParentNoTest(OmSegment* child);

    std::pair<bool, OmSegmentEdge> JoinEdgeFromUser(const OmSegmentEdge& e);
    std::pair<bool, OmSegmentEdge> JoinFromUserAction(const OmID, const OmID);

    void rerootSegmentLists();
    void setGlobalThreshold(OmMST* mst);
    void resetGlobalThreshold(OmMST* mst);

    friend class OmSegmentColorizer;
    friend YAML::Emitter& YAML::operator<<(YAML::Emitter&, const segmentsImpl&);
    friend void YAML::operator>>(const YAML::Node&, segmentsImpl&);
    friend QDataStream& operator<<(QDataStream&, const segmentsImpl&);
    friend QDataStream& operator>>(QDataStream&, segmentsImpl&);
};

