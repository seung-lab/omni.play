#ifndef OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H
#define OM_SEGMENT_CACHE_IMPL_LOW_LEVEL_H

#include "common/omCommon.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegment.h"

#include <QHash>

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

	bool isSegmentSelected( OmSegID segID );
	bool isSegmentSelected( OmSegment * seg );
	void SetAllSelected(bool);
	const OmSegIDsSet& GetSelectedSegmentIds(){
        return mSelectedSet;
	}

	quint32 numberOfSelectedSegments();
	bool AreSegmentsSelected();
	void setSegmentSelected( OmSegID segID, const bool, const bool  );
	void UpdateSegmentSelection( const OmSegIDsSet & ids, const bool);
	void AddToSegmentSelection(const OmSegIDsSet& idsToSelect);
	void RemvoeFromSegmentSelection(const OmSegIDsSet& idsToSelect);

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

protected:
	OmSegmentation *const segmentation_;
	boost::shared_ptr<OmPagingPtrStore> mSegments;

	OmSegID mMaxValue;
	OmSegID getNextValue();

	quint32 mNumSegs;

	bool mAllSelected;
	bool mAllEnabled;

	OmSegIDsSet mEnabledSet;
	OmSegIDsSet mSelectedSet;

	QHash< OmID, QString > segmentCustomNames;
	QHash< OmID, QString > segmentNotes;

	void doSelectedSetInsert( const OmSegID segID, const bool);
	void doSelectedSetRemove( const OmSegID segID);

	void clearCaches();

	OmSegmentGraph mSegmentGraph;

	void addToRecentMap( const OmSegID segID);

private:
	void setSegmentSelectedBatch( OmSegID segID, const bool, const bool );

	friend class SegmentTests;
};

#endif
