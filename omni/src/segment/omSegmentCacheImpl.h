#ifndef OM_SEGMENT_CACHE_IMPL_H
#define OM_SEGMENT_CACHE_IMPL_H

#include "omSegmentCache.h"

class OmSegmentCacheImpl {
public:
	OmSegmentCacheImpl(OmSegmentation * segmentation, OmSegmentCache * cache);
	~OmSegmentCacheImpl();

	OmSegment* AddSegment();
	void AddSegmentsFromChunk(const SegmentDataSet & values, const OmMipChunkCoord & mipCoord);
	OmSegment* AddSegment(SEGMENT_DATA_TYPE value);

	bool isValueAlreadyMappedToSegment( SEGMENT_DATA_TYPE value );

	OmSegment* GetSegmentFromValue(SEGMENT_DATA_TYPE);
	OmSegment* GetSegmentFromID(OmId);

	OmId GetNumSegments();
	OmId GetNumTopSegments();

	bool IsSegmentValid(OmId seg);

	bool isSegmentEnabled( OmId segID );
	void setSegmentEnabled( OmId segID, bool isEnabled );
	void SetAllEnabled(bool);
	OmIds& GetEnabledSegmentIdsRef();
	SegmentDataSet GetEnabledSegmentValues();

	bool isSegmentSelected( OmId segID );
	void setSegmentSelected( OmId segID, bool isSelected );
	void SetAllSelected(bool);
	OmIds& GetSelectedSegmentIdsRef();
	SegmentDataSet GetSelectedSegmentValues();
	quint32 numberOfSelectedSegments();

	QString getSegmentName( OmId segID );
	void setSegmentName( OmId segID, QString name );

	QString getSegmentNote( OmId segID );
	void setSegmentNote( OmId segID, QString note );

	OmId getSegmentationID();

	virtual void addToDirtySegmentList( OmSegment* seg);
	virtual void flushDirtySegments();

	SegmentDataSet getValues( OmSegment * segment  );
	OmIds getIDs( OmSegment * segment );

	OmSegment * findRoot( OmSegment * segment );

	void splitChildLowestThreshold( OmSegment * segment );

	void Save( OmSegment * seg );

	void turnBatchModeOn(const bool batchMode);

 private:
	
	bool mAllSelected;
	bool mAllEnabled;

	OmId getNextSegmentID();
	OmId mMaxSegmentID;

	SEGMENT_DATA_TYPE getNextValue();
	SEGMENT_DATA_TYPE mMaxValue;

	OmIds getIDsHelper( OmSegment * segment );
	SegmentDataSet getValuesHelper( OmSegment * segment );

	int maxDirtySegmentsBeforeFlushing();

	OmSegmentation * mSegmentation;

        OmIds mEnabledSet;
        OmIds mSelectedSet;
	QHash< OmId, QString > segmentCustomNames;
	QHash< OmId, QString > segmentNotes;

	void clearCaches( OmSegment * seg );
	SegmentDataSet cacheOfSelectedSegmentValues;
	QHash< OmId, OmIds > cacheRootNodeToAllChildrenIDs;
	QHash< OmId, SegmentDataSet > cacheRootNodeToAllChildrenValues;

	QSet<OmSegment*> dirtySegments;

	OmSegmentCache * mParentCache;

	bool amInBatchMode;
	bool needToFlush;

	QHash< quint32, QHash<OmId, OmSegment*> > mSegIdToSegPtrHash;
	QHash< quint32, QHash<SEGMENT_DATA_TYPE, OmId> > mValueToSegIdHash;
	quint32 getSegmentPageNum( const OmId segID );
	quint32 getValuePageNum( const SEGMENT_DATA_TYPE value );
	bool doesValuePageExist( const quint32 valuePageNum );
	void LoadValuePage( const quint32 valuePageNum );
	void LoadSegmentPage( const quint32 segPageNum );
	void SavePages();

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCacheImpl & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCacheImpl & sc );
};

#endif
