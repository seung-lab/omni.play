#ifndef OM_SEGMENT_CACHE_H
#define OM_SEGMENT_CACHE_H

#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"
#include "volume/omSegmentation.h" // only needed for friend OmSegmentation::ExportDataFilter()

#include <QSet>
#include <QHash>
#include <QMutex>
#include <QColor>

class OmSegment;
class OmSegmentCacheImpl;
class vtkImageData;  // only needed for friend OmSegmentation::ExportDataFilter()

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
	void UpdateSegmentSelection( const OmSegIDs & idsToSelect);

	QString getSegmentName( OmSegID segID );
	void setSegmentName( OmSegID segID, QString name );

	QString getSegmentNote( OmSegID segID );
	void setSegmentNote( OmSegID segID, QString note );

	OmSegID getSegmentationID();

	void addToDirtySegmentList( OmSegment* seg);
	void flushDirtySegments();

	OmSegment * findRoot( OmSegment * segment );
	OmSegID findRootID( const OmSegID segID );

	void splitChildLowestThreshold( OmSegment * segment );
        void splitTwoChildren(OmSegment * seg1, OmSegment * seg2);

	void JoinTheseSegments( const OmIds & segmentList);
	void UnJoinTheseSegments( const OmIds & segmentList);

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

	OmSegID findRootID_noLock( const OmSegID segID );

	friend class OmSegmentColorizer;
	friend void OmSegmentation::ExportDataFilter(vtkImageData * pImageData);
	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCache & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCache & sc );
};

#endif
