#ifndef OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H
#define OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H

#include "common/omCommon.h"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegment.h"

#include <QHash>

class OmSegmentCache;
class OmSegmentation;

class OmSegmentCacheImplLowLevel {
 public:
	OmSegmentCacheImplLowLevel( OmSegmentation *, OmSegmentCache *);
	virtual ~OmSegmentCacheImplLowLevel();

	void growGraphIfNeeded(OmSegment * newSeg);

	bool isValueAlreadyMappedToSegment(const OmSegID value);
	OmSegment* GetSegmentFromValue(const OmSegID value );

	OmSegID GetNumSegments();
	OmSegID GetNumTopSegments();

	OmSegment * findRoot( OmSegment * segment );
	OmSegment * findRoot(const OmSegID segID);
	OmSegID findRootID( const OmSegID segID );

	bool isSegmentEnabled( OmSegID segID );
	void setSegmentEnabled( OmSegID segID, bool isEnabled );
	void SetAllEnabled(bool);
	OmSegIDsSet& GetEnabledSegmentIdsRef();
	bool AreSegmentsEnabled();
	uint32_t numberOfEnabledSegments();

	bool isSegmentSelected( OmSegID segID );
	bool isSegmentSelected( OmSegment * seg );
	void SetAllSelected(bool);
	OmSegIDsSet& GetSelectedSegmentIdsRef();
	quint32 numberOfSelectedSegments();
	bool AreSegmentsSelected();
	void setSegmentSelected( OmSegID segID, const bool, const bool  );
	void UpdateSegmentSelection( const OmSegIDsSet & ids, const bool);

	QString getSegmentName( OmSegID segID );
	void setSegmentName( OmSegID segID, QString name );

	QString getSegmentNote( OmSegID segID );
	void setSegmentNote( OmSegID segID, QString note );

	OmSegmentation* getSegmentation() { return mSegmentation; }
	OmSegID getSegmentationID();

	void addToDirtySegmentList( OmSegment* seg);
	void flushDirtySegments();

	void turnBatchModeOn(const bool batchMode);

	quint32 getPageSize();
	quint32 getMaxValue();

 protected:
	OmSegmentation * mSegmentation;
	OmSegmentCache * mParentCache;
	OmPagingPtrStore<OmSegment> * mSegments;

	OmSegID mMaxValue;
	OmSegID getNextValue();

	quint32 mNumSegs;

	bool mAllSelected;
	bool mAllEnabled;

        OmSegIDsSet mEnabledSet;
        OmSegIDsSet mSelectedSet;

	QHash< OmId, QString > segmentCustomNames;
	QHash< OmId, QString > segmentNotes;

	void doSelectedSetInsert( const OmSegID segID, const bool);
	void doSelectedSetRemove( const OmSegID segID);

	void clearCaches();

	OmSegmentGraph mSegmentGraph;

	void addToRecentMap( const OmSegID segID);

 private:
	void setSegmentSelectedBatch( OmSegID segID, const bool, const bool );
};

#endif
