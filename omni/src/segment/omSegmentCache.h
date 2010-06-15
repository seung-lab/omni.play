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
	void AddSegmentsFromChunk(const OmSegIDsSet & values, const OmMipChunkCoord & mipCoord,
				  boost::unordered_map< OmSegID, unsigned int> * sizes );
	OmSegment* AddSegment(OmSegID value);

	bool isValueAlreadyMappedToSegment( OmSegID value );

	OmSegment* GetSegmentFromValue(OmSegID);

	OmSegID GetNumSegments();
	OmSegID GetNumTopSegments();

	bool IsSegmentValid(OmSegID seg);

	bool isSegmentEnabled( OmSegID segID );
	void setSegmentEnabled( OmSegID segID, bool isEnabled );
	void SetAllEnabled(bool);
	OmSegIDsSet & GetEnabledSegmentIdsRef();

	bool isSegmentSelected( OmSegID segID );
	bool isSegmentSelected( OmSegment * seg );
	void setSegmentSelected( OmSegID segID, bool isSelected );
	void SetAllSelected(bool);
	OmSegIDsSet & GetSelectedSegmentIdsRef();
	quint32 numberOfSelectedSegments();
	bool AreSegmentsSelected();
	void UpdateSegmentSelection( const OmSegIDsSet & idsToSelect);

	QString getSegmentName( OmSegID segID );
	void setSegmentName( OmSegID segID, QString name );

	QString getSegmentNote( OmSegID segID );
	void setSegmentNote( OmSegID segID, QString note );

	OmSegID getSegmentationID();

	void addToDirtySegmentList( OmSegment* seg);
	void flushDirtySegments();

	OmSegment * findRoot( OmSegment * segment );
	OmSegID findRootID( const OmSegID segID );

        OmSegmentEdge * splitTwoChildren(OmSegment * seg1, OmSegment * seg2);

	OmSegmentEdge * JoinEdge( OmSegmentEdge * e );
	void JoinTheseSegments( const OmSegIDsSet & segmentList);
	void UnJoinTheseSegments( const OmSegIDsSet & segmentList);

	quint32 getPageSize();

	void resetGlobalThreshold( const float stopPoint );

	quint32 getMaxValue();

	OmSegPtrListWithPage * getRootLevelSegIDs( const unsigned int offset, const int numToGet, OmSegIDRootType type, OmSegID startSeg = 0);

	void setAsValidated(const OmSegIDsSet & set, const bool valid);

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
