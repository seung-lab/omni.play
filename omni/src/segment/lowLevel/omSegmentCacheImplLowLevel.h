#ifndef OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H
#define OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H

#include "common/omCommon.h"
#include "segment/lowLevel/omSegmentListByMRU.h"
#include "segment/lowLevel/omPagingStore.h"
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

	bool isSegmentSelected( OmSegID segID );
	bool isSegmentSelected( OmSegment * seg );
	void SetAllSelected(bool);
	OmSegIDsSet& GetSelectedSegmentIdsRef();
	quint32 numberOfSelectedSegments();
	bool AreSegmentsSelected();
	void setSegmentSelected( OmSegID segID, bool isSelected );
	void UpdateSegmentSelection( const OmSegIDsSet & ids);

	QString getSegmentName( OmSegID segID );
	void setSegmentName( OmSegID segID, QString name );

	QString getSegmentNote( OmSegID segID );
	void setSegmentNote( OmSegID segID, QString note );

	OmSegID getSegmentationID();

	void addToDirtySegmentList( OmSegment* seg);
	void flushDirtySegments();

	void turnBatchModeOn(const bool batchMode);

	quint32 getPageSize();
	quint32 getMaxValue();

 protected:
	OmSegmentation * mSegmentation;
	OmSegmentCache * mParentCache;
	OmPagingStore<OmSegment> * mSegments;

	OmSegID mMaxValue;
	OmSegID getNextValue();

	quint32 mNumSegs;

	bool mAllSelected;
	bool mAllEnabled;

        OmSegIDsSet mEnabledSet;
        OmSegIDsSet mSelectedSet;

	QHash< OmId, QString > segmentCustomNames;
	QHash< OmId, QString > segmentNotes;

	void doSelectedSetInsert( const OmSegID segID);
	void doSelectedSetRemove( const OmSegID segID);	

	void clearCaches();

	OmSegmentGraph mSegmentGraph;

	OmSegmentListByMRU mRecentRootActivityMap;
	void addToRecentMap( const OmSegID segID);

 private:
	void setSegmentSelectedBatch( OmSegID segID, bool isSelected );
};

#endif
