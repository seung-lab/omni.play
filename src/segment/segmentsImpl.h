#pragma once

#include "segment/lowLevel/segmentsImplLowLevel.h"
#include "datalayer/archive/segmentation.h"

class segmentEdge;
class OmUserEdges;
class segmentChildren;
class segmentsStore;

class segmentsImpl : public segmentsImplLowLevel {
public:
    segmentsImpl(segmentation*, segmentsStore*);
    virtual ~segmentsImpl();

    void Flush();

    segment* AddSegment();
    segment* AddSegment(segId value);
    segment* GetOrAddSegment(const segId val);

    segmentChildren* Children();

    std::pair<bool, segmentEdge> JoinFromUserAction(const segmentEdge& e);
    segmentEdge SplitEdgeUserAction(const segmentEdge& e);
    segIdsSet JoinTheseSegments(const segIdsSet& segmentList);
    segIdsSet UnJoinTheseSegments(const segIdsSet& segmentList);
    boost::optional<std::string> IsEdgeSplittable(const segmentEdge& e);
    boost::optional<std::string> IsSegmentSplittable(segment* child);
	boost::optional<std::string> IsSegmentCuttable(segment* seg);

    void refreshTree();

    bool AreAnySegmentsInValidList(const segIdsSet& ids);

	std::vector<segmentEdge> CutSegment(segment* seg);
	bool JoinEdges(const std::vector<segmentEdge>& edges);

private:
    OmUserEdges* userEdges_;

    segmentEdge splitChildFromParentNoTest(segment* child);

    std::pair<bool, segmentEdge> JoinEdgeFromUser(const segmentEdge& e);
    std::pair<bool, segmentEdge> JoinFromUserAction(const common::id, const common::id);

    void rerootSegmentLists();
    void setGlobalThreshold(OmMST* mst);
    void resetGlobalThreshold(OmMST* mst);

    friend class segmentColorizer;
    friend YAML::Emitter& YAML::operator<<(YAML::Emitter&, const segmentsImpl&);
    friend void YAML::operator>>(const YAML::Node&, segmentsImpl&);
    friend QDataStream& operator<<(QDataStream&, const segmentsImpl&);
    friend QDataStream& operator>>(QDataStream&, segmentsImpl&);
};

