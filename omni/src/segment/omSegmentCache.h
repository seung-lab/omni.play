#ifndef OM_SEGMENT_CACHE_H
#define OM_SEGMENT_CACHE_H

#include "common/omStd.h"
#include "segment/omSegment.h"
#include "utility/omHdf5Path.h"

#include <QSet>
#include <QHash>
#include <QMutex>

class OmSegment;
class OmSegmentation;
class OmSegmentCacheImpl;

class OmSegmentCache {
public:
	OmSegmentCache(OmSegmentation * segmentation);
	~OmSegmentCache();

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

	void addToDirtySegmentList( OmSegment* seg);
	void flushDirtySegments();

	SegmentDataSet getValues( OmSegment * segment  );
	OmIds getIDs( OmSegment * segment );

	OmIds getIDsHelper( OmSegment * segment );
	OmSegment * findRoot( OmSegment * segment );

private:
	QMutex mMutex;
	
	OmSegmentCacheImpl * mImpl;

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCache & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCache & sc );
};

#endif
