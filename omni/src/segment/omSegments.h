#ifndef OM_SEGMENT_CACHE_H
#define OM_SEGMENT_CACHE_H

#include "volume/omVolumeTypes.hpp"
#include "segment/omSegmentPointers.h"

#include <zi/mutex.hpp>

class OmChunkCoord;
class OmSegment;
class OmSegmentsImpl;
class OmSegmentation;
class OmSegmentChildren;

class OmSegments {
public:
    OmSegments(OmSegmentation* segmentation);
    ~OmSegments();

    void Flush();

    OmSegment* AddSegment();
    OmSegment* AddSegment(OmSegID value);
    OmSegment* GetSegment(const OmSegID);
    OmSegment* GetOrAddSegment(const OmSegID);

    bool IsSegmentValid(OmSegID seg);

    OmSegID GetNumSegments();
    OmSegID GetNumTopSegments();

    OmSegmentChildren* Children();

    bool isSegmentEnabled(OmSegID segID);
    void setSegmentEnabled(OmSegID segID, bool isEnabled);
    OmSegIDsSet & GetEnabledSegmentIds();
    bool AreSegmentsEnabled();

    bool IsSegmentSelected(OmSegID segID);
    bool IsSegmentSelected(OmSegment* seg);
    void setSegmentSelected(OmSegID segID, const bool, const bool);
    const OmSegIDsSet& GetSelectedSegmentIds();
    quint32 numberOfSelectedSegments();
    bool AreSegmentsSelected();
    void UpdateSegmentSelection(const OmSegIDsSet& idsToSelect, const bool);
    void AddToSegmentSelection(const OmSegIDsSet& idsToSelect);
    void RemoveFromSegmentSelection(const OmSegIDsSet& idsToSelect);

    QString getSegmentName(OmSegID segID);
    void setSegmentName(OmSegID segID, QString name);

    QString getSegmentNote(OmSegID segID);
    void setSegmentNote(OmSegID segID, QString note);

    OmSegID GetSegmentationID();

    OmSegment* findRoot(OmSegment* segment);
    OmSegment* findRoot(const OmSegID segID);
    OmSegID findRootID(const OmSegID segID);
    OmSegID findRootID(OmSegment* segment);

    std::pair<bool, OmSegmentEdge> JoinEdge(const OmSegmentEdge& e);
    OmSegmentEdge SplitEdge(const OmSegmentEdge& e);
    OmSegIDsSet JoinTheseSegments(const OmSegIDsSet& segmentList);
    OmSegIDsSet UnJoinTheseSegments(const OmSegIDsSet& segmentList);

    uint32_t getPageSize();

    uint32_t getMaxValue();

    void refreshTree();

    bool AreAnySegmentsInValidList(const OmSegIDsSet& ids);

    uint64_t MSTfreshness() const;

    OmSegmentEdge SplitSegment(const OmSegID, const DataCoord, const DataCoord)
    {
        printf("Watershed-based split segment is NYI.\n");
        return OmSegmentEdge();
    }

    void UnSplitSegment(const OmSegmentEdge &)
    {
        printf("Watershed-based unsplitSegment is NYI.\n");
    }

private:
    zi::mutex mutex_;
    OmSegmentation *const segmentation_;
    boost::scoped_ptr<OmSegmentsImpl> mImpl;

    friend class OmSegmentColorizer;
    friend class SegmentTests;

    friend QDataStream &operator<<(QDataStream& out, const OmSegments& sc);
    friend QDataStream &operator>>(QDataStream& in, OmSegments& sc);
};

#endif
