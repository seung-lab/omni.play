#ifndef OM_SEGMENT_CACHE_IMPL_H
#define OM_SEGMENT_CACHE_IMPL_H

#include "segment/omSegmentCache.h"
#include "utility/DynamicTree.h"
#include <QMap>

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

	bool isSegmentSelected( OmId segID );
	bool isSegmentSelected( OmSegment * seg );
	void setSegmentSelected( OmId segID, bool isSelected );
	void SetAllSelected(bool);
	OmIds& GetSelectedSegmentIdsRef();
	quint32 numberOfSelectedSegments();
	bool AreSegmentsSelected();

	QString getSegmentName( OmId segID );
	void setSegmentName( OmId segID, QString name );

	QString getSegmentNote( OmId segID );
	void setSegmentNote( OmId segID, QString note );

	OmId getSegmentationID();

	void addToDirtySegmentList( OmSegment* seg);
	void flushDirtySegments();

	OmSegment * findRoot( OmSegment * segment );

	void splitChildLowestThreshold( OmSegment * segment );
	void splitTwoChildren(OmSegment * seg1, OmSegment * seg2);
	void splitChildFromParent( OmSegment * child );

	void turnBatchModeOn(const bool batchMode);
	
	void Join(OmSegment * parent, OmSegment * childUnknownLevel, double threshold);
	void clearAllJoins();

	quint32 getPageSize() { return mPageSize; }

	void setSegmentListDirectCache( const OmMipChunkCoord & chunkCoord,
					QList< OmSegment* > segmentsToDraw );
	bool segmentListDirectCacheHasCoord( const OmMipChunkCoord & chunkCoord );
	QList< OmSegment* > getSegmentListDirectCache( const OmMipChunkCoord & chunkCoord );

	OmColor getVoxelColorForView2d( const SEGMENT_DATA_TYPE val, const bool showOnlySelectedSegments );

	void reloadDendrogram( const quint32 * dend, const float * dendValues, 
			       const int size, const float stopPoint );

 private:
	bool mAllSelected;
	bool mAllEnabled;

	SEGMENT_DATA_TYPE getNextValue();
	SEGMENT_DATA_TYPE mMaxValue;
	quint32 mNumSegs;

	OmSegmentation * mSegmentation;

        OmIds mEnabledSet;
        OmIds mSelectedSet;
	QHash< OmId, QString > segmentCustomNames;
	QHash< OmId, QString > segmentNotes;

	QSet<quint32> dirtySegmentPages;

	OmSegmentCache * mParentCache;

	bool amInBatchMode;
	bool needToFlush;

	quint32 mPageSize;
	QHash< quint32, OmSegment** > mValueToSegPtrHash;
	QSet< quint32 > validPageNumbers;
	QSet< quint32 > loadedPageNumbers;
	quint32 getValuePageNum( const SEGMENT_DATA_TYPE value );
	void LoadValuePage( const quint32 valuePageNum );
	void SaveAllPages();
	void SaveDirtySegmentPages();
	void doSaveSegmentPage( const quint32 segPageNum );
	bool mAllPagesLoaded;

	OmSegment* GetSegmentFromValueFast(SEGMENT_DATA_TYPE value);

	QMap< OmMipChunkCoord, QList< OmSegment* > > cacheDirectSegmentList;
	void clearCaches();
	void invalidateCachedRootFreshness();
	void invalidateCachedColorFreshness();
	quint32 mCachedRootFreshness;
	quint32 mCachedColorFreshness;

	DynamicTree<SEGMENT_DATA_TYPE> ** tree;
	void reloadDynamicTree();

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCacheImpl & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCacheImpl & sc );

	void JoinDynamicTree( const OmId, const OmId, const double );

	int clamp(int c) {
		if (c > 255) {
			return 255;
		}
		return c;
	}

};

#endif
