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

 protected:
	QHash<OmId, OmSegment*> mSegIdToSegPtrHash;

 private:
	
	bool mAllSelected;
	bool mAllEnabled;

	OmSegment* LoadSegment(OmId);
	OmSegment* doLoadSegment(OmId);
	OmHdf5Path getSegmentPath( OmId segmentID );
	OmHdf5Path getSegmentPath( OmSegment * seg );
	OmId getNextSegmentID();
	OmId mMaxSegmentID;

	QHash<SEGMENT_DATA_TYPE, OmId> mValueToSegIdHash;
	OmId LoadValue(SEGMENT_DATA_TYPE);
	OmId doLoadValue(SEGMENT_DATA_TYPE);
	OmHdf5Path getValuePath( SEGMENT_DATA_TYPE value );
	OmHdf5Path getValuePath( OmSegment * seg );
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

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCacheImpl & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCacheImpl & sc );
};

#endif
