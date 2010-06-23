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

	bool isValueAlreadyMappedToSegment(const OmSegID value);
	OmSegment* GetSegmentFromValue(const OmSegID value );

	OmSegID GetNumSegments();
	OmSegID GetNumTopSegments();

	bool isSegmentEnabled( OmSegID segID );
	void setSegmentEnabled( OmSegID segID, bool isEnabled );
	void SetAllEnabled(bool);
	OmSegIDsSet& GetEnabledSegmentIdsRef();

	bool isSegmentSelected( OmSegID segID );
	bool isSegmentSelected( OmSegment * seg );
	void setSegmentSelected( OmSegID segID, bool isSelected );
	void SetAllSelected(bool);
	OmSegIDsSet& GetSelectedSegmentIdsRef();
	quint32 numberOfSelectedSegments();
	bool AreSegmentsSelected();
	void UpdateSegmentSelection( const OmSegIDsSet & ids);

	QString getSegmentName( OmSegID segID );
	void setSegmentName( OmSegID segID, QString name );

	QString getSegmentNote( OmSegID segID );
	void setSegmentNote( OmSegID segID, QString note );

	OmSegID getSegmentationID();

	void addToDirtySegmentList( OmSegment* seg);
	void flushDirtySegments();

	OmSegment * findRoot( OmSegment * segment );
	OmSegID findRootID( const OmSegID segID );

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

	void clearCaches();

	OmSegmentGraph mSegmentGraph;
	void initialize();
	void resetGlobalThreshold( const float stopPoint );

	void rerootSegmentLists();
	void rerootSegmentList( OmSegIDsSet & set );

	virtual void setSegmentSelectedBatch( OmSegID segID, bool isSelected ) = 0;
	virtual void updateSizeListsFromJoin( OmSegment * root, OmSegment * child ) = 0;

	OmSegmentListByMRU mRecentRootActivityMap;

	void buildSegmentSizeLists();

 private:
	void doResetGlobalThreshold( const quint32 * dend, 
				     const float * dendValues, 
				     quint8 * edgeDisabledByUser,
				     quint8 * edgeWasJoined,
				     quint8 * edgeForceJoin,
				     const int size, 
				     const float stopPoint );
	bool JoinInternal( const OmSegID parentID, const OmSegID childUnknownDepthID, 
			   const float threshold, const int edgeNumber);
	bool splitChildFromParentInternal( const OmSegID childID );

};

#endif
