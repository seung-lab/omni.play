#pragma once

#include "volume/omVolumeTypes.hpp"
#include "segment/omSegmentPointers.h"
#include "datalayer/archive/segmentation.h"

#include <zi/mutex.hpp>

class OmChunkCoord;
class OmSegment;
class OmSegmentChildren;
class OmSegmentation;
class OmSegmentsImpl;
class OmSegmentsStore;
class OmSegmentDataWrapper;

class OmSegments {
public:
    OmSegments(OmSegmentation* segmentation);
    ~OmSegments();

    void StartCaches();
    void Flush();

    OmSegment* AddSegment();
    OmSegment* AddSegment(OmSegID value);
    OmSegment* GetSegment(const OmSegID);
    OmSegment* GetSegmentUnsafe(const OmSegID);
    OmSegment* GetOrAddSegment(const OmSegID);

    bool IsSegmentValid(OmSegID seg);

    OmSegID GetNumSegments();
    OmSegID GetNumTopSegments();

    OmSegmentChildren* Children();

    bool isSegmentEnabled(OmSegID segID);
    void setSegmentEnabled(OmSegID segID, bool isEnabled);
    OmSegIDsSet GetEnabledSegmentIDs();
    bool AreSegmentsEnabled();

    bool IsSegmentSelected(OmSegID segID);
    bool IsSegmentSelected(OmSegment* seg);
    void setSegmentSelected(OmSegID segID, const bool, const bool);
    const OmSegIDsSet GetSelectedSegmentIDs();
    uint32_t NumberOfSelectedSegments();
    bool AreSegmentsSelected();
    void UpdateSegmentSelection(const OmSegIDsSet& idsToSelect, const bool);
    void AddToSegmentSelection(const OmSegIDsSet& idsToSelect);
    void RemoveFromSegmentSelection(const OmSegIDsSet& idsToSelect);

    QString getSegmentName(OmSegID segID);
    void setSegmentName(OmSegID segID, QString name);

    QString getSegmentNote(OmSegID segID);
    void setSegmentNote(OmSegID segID, QString note);

    OmSegID GetSegmentationID();

    boost::optional<std::string> IsEdgeSplittable(const OmSegmentEdge& e);
    boost::optional<std::string> IsSegmentSplittable(OmSegment* child);
	boost::optional<std::string> IsSegmentCuttable(OmSegment* seg);

    OmSegment* findRoot(OmSegment* segment);
    OmSegment* findRoot(const OmSegID segID);
    OmSegID findRootID(const OmSegID segID);
    OmSegID findRootID(OmSegment* segment);
    OmSegID findRootIDnoCache(const OmSegID segID);

    std::pair<bool, OmSegmentEdge> JoinEdge(const OmSegmentEdge& e);
    OmSegmentEdge SplitEdge(const OmSegmentEdge& e);
    OmSegIDsSet JoinTheseSegments(const OmSegIDsSet& segmentList);
    OmSegIDsSet UnJoinTheseSegments(const OmSegIDsSet& segmentList);

    // Your method
    // void GrowSelection(OmSegmentDataWrapper& sdw);
    // void ShrinkSelection(OmSegmentDataWrapper& sdw);

	std::vector<OmSegmentEdge> CutSegment(OmSegment* seg);
	bool JoinEdges(const std::vector<OmSegmentEdge>& edges);

    uint32_t getPageSize();

    uint32_t getMaxValue();

    void refreshTree();

    bool AreAnySegmentsInValidList(const OmSegIDsSet& ids);

    uint64_t MSTfreshness() const;

private:
    zi::mutex mutex_;

    OmSegmentation *const segmentation_;

    boost::scoped_ptr<OmSegmentsStore> store_;
    boost::scoped_ptr<OmSegmentsImpl> impl_;

    friend class OmSegmentColorizer;
    friend class SegmentTests;

    friend YAML::Emitter &YAML::operator<<(YAML::Emitter& out, const OmSegments& sc);
    friend void YAML::operator>>(const YAML::Node& in, OmSegments& sc);
    friend QDataStream &operator<<(QDataStream& out, const OmSegments& sc);
    friend QDataStream &operator>>(QDataStream& in, OmSegments& sc);
};

