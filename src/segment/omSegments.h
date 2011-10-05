#pragma once

#include "volume/omVolumeTypes.hpp"
#include "segment/omSegmentPointers.h"
#include "datalayer/archive/segmentation.h"

#include <zi/mutex.hpp>

class OmSegment;
class OmSegmentChildren;
class segmentation;
class segmentsImpl;
class segmentsStore;

class segments {
public:
    segments(segmentation* segmentation);
    ~segments();

    void StartCaches();
    void Flush();

    OmSegment* AddSegment();
    OmSegment* AddSegment(segId value);
    OmSegment* GetSegment(const segId);
    OmSegment* GetSegmentUnsafe(const segId);
    OmSegment* GetOrAddSegment(const segId);

    bool IsSegmentValid(segId seg);

    segId GetNumSegments();
    segId GetNumTopSegments();

    OmSegmentChildren* Children();

    bool isSegmentEnabled(segId segID);
    void setSegmentEnabled(segId segID, bool isEnabled);
    segIdsSet GetEnabledSegmentIDs();
    bool AreSegmentsEnabled();

    bool IsSegmentSelected(segId segID);
    bool IsSegmentSelected(OmSegment* seg);
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

    boost::optional<std::string> IsEdgeSplittable(const OmSegmentEdge& e);
    boost::optional<std::string> IsSegmentSplittable(OmSegment* child);
	boost::optional<std::string> IsSegmentCuttable(OmSegment* seg);

    OmSegment* findRoot(OmSegment* segment);
    OmSegment* findRoot(const segId segID);
    segId findRootID(const segId segID);
    segId findRootID(OmSegment* segment);
    segId findRootIDnoCache(const segId segID);

    std::pair<bool, OmSegmentEdge> JoinEdge(const OmSegmentEdge& e);
    OmSegmentEdge SplitEdge(const OmSegmentEdge& e);
    segIdsSet JoinTheseSegments(const segIdsSet& segmentList);
    segIdsSet UnJoinTheseSegments(const segIdsSet& segmentList);

	std::vector<OmSegmentEdge> CutSegment(OmSegment* seg);
	bool JoinEdges(const std::vector<OmSegmentEdge>& edges);

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

    friend class OmSegmentColorizer;
    friend class SegmentTests;

    friend YAML::Emitter &YAML::operator<<(YAML::Emitter& out, const segments& sc);
    friend void YAML::operator>>(const YAML::Node& in, segments& sc);
    friend QDataStream &operator<<(QDataStream& out, const segments& sc);
    friend QDataStream &operator>>(QDataStream& in, segments& sc);
};

