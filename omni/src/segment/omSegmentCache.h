#ifndef OM_SEGMENT_CACHE_H
#define OM_SEGMENT_CACHE_H

#include "volume/omVolumeTypes.hpp"
#include "segment/omSegmentPointers.h"

#include <zi/mutex.hpp>

class OmMipChunkCoord;
class OmSegment;
class OmSegmentCacheImpl;
class OmSegmentation;

class OmSegmentCache {
public:
	OmSegmentCache(OmSegmentation * segmentation);
	~OmSegmentCache();

	void Flush();

	void turnBatchModeOn( const bool batchMode );

	OmSegment* AddSegment();
	OmSegment* AddSegment(OmSegID value);
	OmSegment* GetSegment(const OmSegID);
	OmSegment* GetOrAddSegment(const OmSegID);

	bool IsSegmentValid(OmSegID seg);

	OmSegID GetNumSegments();
	OmSegID GetNumTopSegments();

	bool isSegmentEnabled( OmSegID segID );
	void setSegmentEnabled( OmSegID segID, bool isEnabled );
	void SetAllEnabled(bool);
	OmSegIDsSet & GetEnabledSegmentIds();
	bool AreSegmentsEnabled();

	bool IsSegmentSelected( OmSegID segID );
	bool IsSegmentSelected( OmSegment * seg );
	void setSegmentSelected( OmSegID segID, const bool, const bool );
	void SetAllSelected(bool);
	OmSegIDsSet & GetSelectedSegmentIds();
	quint32 numberOfSelectedSegments();
	bool AreSegmentsSelected();
	void UpdateSegmentSelection( const OmSegIDsSet & idsToSelect, const bool);

	QString getSegmentName( OmSegID segID );
	void setSegmentName( OmSegID segID, QString name );

	QString getSegmentNote( OmSegID segID );
	void setSegmentNote( OmSegID segID, QString note );

	OmSegID getSegmentationID();

	OmSegment * findRoot( OmSegment * segment );
	OmSegment * findRoot( const OmSegID segID );
	OmSegID findRootID( const OmSegID segID );
	OmSegID findRootID(OmSegment* segment);

	OmSegmentEdge findClosestCommonEdge(OmSegment *, OmSegment *);

	std::pair<bool, OmSegmentEdge> JoinEdge( const OmSegmentEdge & e );
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

	void refreshTree();

	quint64 getSizeRootAndAllChildren( OmSegment * segUnknownDepth );

private:
	zi::mutex mutex_;
	OmSegmentation *const mSegmentation;
	boost::shared_ptr<OmSegmentCacheImpl> mImpl;

	friend class OmSegmentColorizer;
	friend class SegmentTests;

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCache & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCache & sc );
};

#endif
