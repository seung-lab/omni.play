#ifndef OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H
#define OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H

#include "common/omCommon.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegment.h"

#include <QHash>

class OmSegmentSelection;
class OmSegmentCache;
class OmSegmentation;
class OmPagingPtrStore;

class OmSegmentCacheImplLowLevel {
public:
    OmSegmentCacheImplLowLevel( OmSegmentation *);
    virtual ~OmSegmentCacheImplLowLevel();

    void growGraphIfNeeded(OmSegment * newSeg);

    OmSegment* GetSegment(const OmSegID value );

    OmSegID GetNumSegments();
    OmSegID GetNumTopSegments();

    OmSegment * findRoot( OmSegment * segment );
    OmSegment * findRoot(const OmSegID segID);
    OmSegID findRootID( const OmSegID segID );
    OmSegID findRootID(OmSegment* segment);

    bool isSegmentEnabled( OmSegID segID );
    void setSegmentEnabled( OmSegID segID, bool isEnabled );
    void SetAllEnabled(bool);
    OmSegIDsSet& GetEnabledSegmentIdsRef();
    bool AreSegmentsEnabled();
    uint32_t numberOfEnabledSegments();

    QString getSegmentName( OmSegID segID );
    void setSegmentName( OmSegID segID, QString name );

    QString getSegmentNote( OmSegID segID );
    void setSegmentNote( OmSegID segID, QString note );

    OmSegmentation* GetSegmentation() { return segmentation_; }
    OmSegID GetSegmentationID();

    void turnBatchModeOn(const bool batchMode);

    quint32 getPageSize();
    uint32_t getMaxValue();

    OmSegmentCache* SegmentCache();

    inline OmSegmentSelection& SegmentSelection(){
        return *segmentSelection_;
    }

protected:
    OmSegmentation *const segmentation_;
    boost::shared_ptr<OmPagingPtrStore> mSegments;

    OmSegID mMaxValue;
    OmSegID getNextValue();

    uint32_t mNumSegs;

    boost::scoped_ptr<OmSegmentSelection> segmentSelection_;

    QHash< OmID, QString > segmentCustomNames;
    QHash< OmID, QString > segmentNotes;

    void touchFreshness();

    bool mAllEnabled;
    OmSegIDsSet mEnabledSet;

    OmSegmentGraph mSegmentGraph;

    void addToRecentMap( const OmSegID segID);

private:
    friend class OmSegmentSelection;
    friend class SegmentTests;
};

#endif
