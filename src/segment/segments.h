#pragma once

#include "volume/volumeTypes.h"
#include "segment/segmentPointers.h"
#include "datalayer/archive/segmentation.h"

#include <zi/mutex.hpp>

class segment;
class segmentChildren;
class segmentation;
class segmentsImpl;
class segmentsStore;

class segments {
public:
    segments(segmentation* segmentation);
    ~segments();

    void StartCaches();
    void Flush();

    segment* AddSegment();
    segment* AddSegment(common::segId value);
    segment* GetSegment(const common::segId);
    segment* GetSegmentUnsafe(const common::segId);
    segment* GetOrAddSegment(const common::segId);

    bool IsSegmentValid(common::segId seg);

    common::segId GetNumSegments();
    common::segId GetNumTopSegments();

    segmentChildren* Children();

    bool isSegmentEnabled(common::segId segID);
    void setSegmentEnabled(common::segId segID, bool isEnabled);
    common::segIdsSet GetEnabledSegmentIDs();
    bool AreSegmentsEnabled();

    bool IsSegmentSelected(common::segId segID);
    bool IsSegmentSelected(segment* seg);
    void setSegmentSelected(common::segId segID, const bool, const bool);
    const common::segIdsSet GetSelectedSegmentIDs();
    uint32_t NumberOfSelectedSegments();
    bool AreSegmentsSelected();
    void UpdateSegmentSelection(const common::segIdsSet& idsToSelect, const bool);
    void AddToSegmentSelection(const common::segIdsSet& idsToSelect);
    void RemoveFromSegmentSelection(const common::segIdsSet& idsToSelect);

    std::string getSegmentName(common::segId segID);
    void setSegmentName(common::segId segID, std::string name);

    std::string getSegmentNote(common::segId segID);
    void setSegmentNote(common::segId segID, std::string note);

    common::segId GetSegmentationID();

    boost::optional<std::string> IsEdgeSplittable(const segmentEdge& e);
    boost::optional<std::string> IsSegmentSplittable(segment* child);
	boost::optional<std::string> IsSegmentCuttable(segment* seg);

    segment* findRoot(segment* segment);
    segment* findRoot(const common::segId segID);
    common::segId findRootID(const common::segId segID);
    common::segId findRootID(segment* segment);
    common::segId findRootIDnoCache(const common::segId segID);

    std::pair<bool, segmentEdge> JoinEdge(const segmentEdge& e);
    segmentEdge SplitEdge(const segmentEdge& e);
    common::segIdsSet JoinTheseSegments(const common::segIdsSet& segmentList);
    common::segIdsSet UnJoinTheseSegments(const common::segIdsSet& segmentList);

	std::vector<segmentEdge> CutSegment(segment* seg);
	bool JoinEdges(const std::vector<segmentEdge>& edges);

    uint32_t getPageSize();

    uint32_t getMaxValue();

    void refreshTree();

    bool AreAnySegmentsInValidList(const common::segIdsSet& ids);

    uint64_t MSTfreshness() const;

    inline const segmentation * getSegmentation() const {
        return segmentation_;
    }
    
private:
    zi::mutex mutex_;

    segmentation *const segmentation_;

    boost::scoped_ptr<segmentsStore> store_;
    boost::scoped_ptr<segmentsImpl> impl_;

    friend class segmentColorizer;
    friend class SegmentTests;

    friend YAML::Emitter &YAML::operator<<(YAML::Emitter& out, const segments& sc);
    friend void YAML::operator>>(const YAML::Node& in, segments& sc);
    friend QDataStream &operator<<(QDataStream& out, const segments& sc);
    friend QDataStream &operator>>(QDataStream& in, segments& sc);
};

