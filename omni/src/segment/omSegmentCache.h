#ifndef OM_SEGMENT_CACHE_H
#define OM_SEGMENT_CACHE_H

#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"

#include <QMutex>

class OmMipChunkCoord;
class OmSegment;
class OmSegmentCacheImpl;
class OmSegmentation;
class vtkImageData;

class OmSegmentCache {
public:
	OmSegmentCache(OmSegmentation * segmentation);
	~OmSegmentCache();

	void turnBatchModeOn( const bool batchMode );

	OmSegment* AddSegment();
	void AddSegmentsFromChunk(const OmSegIDsSet & values, const OmMipChunkCoord & mipCoord,
				  boost::unordered_map< OmSegID, unsigned int> * sizes );
	OmSegment* AddSegment(OmSegID value);

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

	quint32 getMaxValue();
	quint64 getSegmentListSize(OmSegIDRootType type);

	OmSegPtrListWithPage * getRootLevelSegIDs( const unsigned int offset, const int numToGet, OmSegIDRootType type, OmSegID startSeg = 0);

	void setAsValidated(OmSegment * segment, const bool valid);

	void ExportDataFilter(vtkImageData * pImageData);

	void refreshTree();

private:
	QMutex mMutex;
	OmSegmentation * mSegmentation;
	OmSegmentCacheImpl * mImpl;

	friend class OmSegmentColorizer;

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCache & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCache & sc );
};

#endif
