#ifndef OM_SEGMENT_CACHE_H
#define OM_SEGMENT_CACHE_H

#include "common/omStd.h"
#include "segment/omSegment.h"
#include "utility/omHdf5Path.h"

#include <QSet>
#include <QHash>
#include <QMutex>
#include <QColor>

class OmSegment;
class OmSegmentation;
class OmSegmentCacheImpl;

class OmSegmentCache {
public:
	OmSegmentCache(OmSegmentation * segmentation);
	~OmSegmentCache();

	void turnBatchModeOn( const bool batchMode );

	OmSegment* AddSegment();
	void AddSegmentsFromChunk(const SegmentDataSet & values, const OmMipChunkCoord & mipCoord);
	OmSegment* AddSegment(SEGMENT_DATA_TYPE value);

	bool isValueAlreadyMappedToSegment( SEGMENT_DATA_TYPE value );

	OmSegment* GetSegmentFromValue(SEGMENT_DATA_TYPE);

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

	void JoinAllSegmentsInSelectedList();

	quint32 getPageSize();

	void setSegmentListDirectCache( const OmMipChunkCoord & chunkCoord,
					QList< OmSegment* > segmentsToDraw );
	bool segmentListDirectCacheHasCoord( const OmMipChunkCoord & chunkCoord );
	QList< OmSegment* > getSegmentListDirectCache( const OmMipChunkCoord & chunkCoord );

	void reloadDendrogram( const quint32 * dend, const float * dendValues, 
			       const int size, const float stopPoint );

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
