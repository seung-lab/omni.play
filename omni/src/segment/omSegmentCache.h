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

	OmSegment* GetSegment(const OmSegID);

	bool IsSegmentValid(OmSegID seg);

	OmSegID GetNumSegments();
	OmSegID GetNumTopSegments();

	bool isSegmentEnabled( OmSegID segID );
	void setSegmentEnabled( OmSegID segID, bool isEnabled );
	void SetAllEnabled(bool);
	OmSegIDsSet & GetEnabledSegmentIds();

	bool IsSegmentSelected( OmSegID segID );
	bool IsSegmentSelected( OmSegment * seg );
	void setSegmentSelected( OmSegID segID, bool isSelected );
	void SetAllSelected(bool);
	OmSegIDsSet & GetSelectedSegmentIds();
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
	OmSegment * findRoot( const OmSegID segID );
	OmSegID findRootID( const OmSegID segID );

	OmSegmentEdge findClosestCommonEdge(OmSegment *, OmSegment *);

	OmSegmentEdge JoinEdge( const OmSegmentEdge & e );
	OmSegmentEdge SplitEdge( const OmSegmentEdge & e );
	void JoinTheseSegments( const OmSegIDsSet & segmentList);
	void UnJoinTheseSegments( const OmSegIDsSet & segmentList);

	quint32 getPageSize();

	quint32 getMaxValue();
	quint64 getSegmentListSize(OmSegIDRootType type);

	OmSegPtrListWithPage * getRootLevelSegIDs(const unsigned int offset, 
						  const int numToGet, 
						  OmSegIDRootType type, 
						  OmSegID startSeg = 0);

	void setAsValidated(OmSegment * segment, const bool valid);

	void ExportDataFilter(vtkImageData * pImageData);

	void refreshTree();

	quint64 getSizeRootAndAllChildren( OmSegment * segUnknownDepth );

private:
	QMutex mMutex;
	OmSegmentation *const mSegmentation;
	OmSegmentCacheImpl *const mImpl;

	friend class OmSegmentColorizer;

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCache & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCache & sc );
};

#endif
