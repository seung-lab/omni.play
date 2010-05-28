#ifndef OM_SEGMENT_CACHE_H
#define OM_SEGMENT_CACHE_H

#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"

#include <QSet>
#include <QHash>
#include <QMutex>
#include <QColor>

class OmSegment;
class OmSegmentCacheImpl;
class OmSegmentation;

class OmSegmentCache {
public:
	OmSegmentCache(OmSegmentation * segmentation);
	~OmSegmentCache();

	void turnBatchModeOn( const bool batchMode );

	OmSegment* AddSegment();
	void AddSegmentsFromChunk(const OmSegIDs & values, const OmMipChunkCoord & mipCoord);
	OmSegment* AddSegment(OmSegID value);

	bool isValueAlreadyMappedToSegment( OmSegID value );

	OmSegment* GetSegmentFromValue(OmSegID);

	OmSegID GetNumSegments();
	OmSegID GetNumTopSegments();

	bool IsSegmentValid(OmSegID seg);

	bool isSegmentEnabled( OmSegID segID );
	void setSegmentEnabled( OmSegID segID, bool isEnabled );
	void SetAllEnabled(bool);
	OmSegIDs & GetEnabledSegmentIdsRef();

	bool isSegmentSelected( OmSegID segID );
	bool isSegmentSelected( OmSegment * seg );
	void setSegmentSelected( OmSegID segID, bool isSelected );
	void SetAllSelected(bool);
	OmSegIDs & GetSelectedSegmentIdsRef();
	quint32 numberOfSelectedSegments();
	bool AreSegmentsSelected();
	void UpdateSegmentSelection( const OmSegIDs & ids, const bool areSelected );

	QString getSegmentName( OmSegID segID );
	void setSegmentName( OmSegID segID, QString name );

	QString getSegmentNote( OmSegID segID );
	void setSegmentNote( OmSegID segID, QString note );

	OmSegID getSegmentationID();

	void addToDirtySegmentList( OmSegment* seg);
	void flushDirtySegments();

	OmSegment * findRoot( OmSegment * segment );

	void splitChildLowestThreshold( OmSegment * segment );
        void splitTwoChildren(OmSegment * seg1, OmSegment * seg2);

	void JoinAllSegmentsInSelectedList();

	quint32 getPageSize();

	void setSegmentListDirectCache( const OmMipChunkCoord & chunkCoord,
					std::vector< OmSegment* > & segmentsToDraw );
	bool segmentListDirectCacheHasCoord( const OmMipChunkCoord & chunkCoord );
	const OmSegPtrs & getSegmentListDirectCache( const OmMipChunkCoord & chunkCoord );

	void resetGlobalThreshold( const float stopPoint );

	quint32 getMaxValue();

private:
	QMutex mMutex;
	
	OmSegmentCacheImpl * mImpl;
	OmSegmentation * mSegmentation;
	quint32 mPageSize;

	friend class OmSegmentColorizer;
	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCache & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCache & sc );
};

#endif
