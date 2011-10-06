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
    segment* AddSegment(segId value);
    segment* GetSegment(const segId);
    segment* GetSegmentUnsafe(const segId);
    segment* GetOrAddSegment(const segId);

    bool IsSegmentValid(segId seg);

    segId GetNumSegments();
    segId GetNumTopSegments();

    segmentChildren* Children();

    bool isSegmentEnabled(segId segID);
    void setSegmentEnabled(segId segID, bool isEnabled);
    segIdsSet GetEnabledSegmentIDs();
    bool AreSegmentsEnabled();

    bool IsSegmentSelected(segId segID);
    bool IsSegmentSelected(segment* seg);
    void setSegmentSelected(segId segID, const bool, const bool);
    const segIdsSet GetSelectedSegmentIDs();
    uint32_t NumberOfSelectedSegments();
    bool AreSegmentsSelected();
    void UpdateSegmentSelection(const segIdsSet& idsToSelect, const bool);
    void AddToSegmentSelection(const segIdsSet& idsToSelect);
    void RemoveFromSegmentSelection(const segIdsSet& idsToSelect);

    QString getSegmentName(segId segID);
    void setSegmentName(segId segID, QString name);

    QString getSegmentNote(segId segID);
    void setSegmentNote(segId segID, QString note);

    segId GetSegmentationID();

    boost::optional<std::string> IsEdgeSplittable(const segmentEdge& e);
    boost::optional<std::string> IsSegmentSplittable(segment* child);
	boost::optional<std::string> IsSegmentCuttable(segment* seg);

    segment* findRoot(segment* segment);
    segment* findRoot(const segId segID);
    segId findRootID(const segId segID);
    segId findRootID(segment* segment);
    segId findRootIDnoCache(const segId segID);

    std::pair<bool, segmentEdge> JoinEdge(const segmentEdge& e);
    segmentEdge SplitEdge(const segmentEdge& e);
    segIdsSet JoinTheseSegments(const segIdsSet& segmentList);
    segIdsSet UnJoinTheseSegments(const segIdsSet& segmentList);

	std::vector<segmentEdge> CutSegment(segment* seg);
	bool JoinEdges(const std::vector<segmentEdge>& edges);

    uint32_t getPageSize();

    uint32_t getMaxValue();

    void refreshTree();

    bool AreAnySegmentsInValidList(const segIdsSet& ids);

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

