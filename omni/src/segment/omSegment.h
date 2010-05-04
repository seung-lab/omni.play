#ifndef OM_SEGMENT_H
#define OM_SEGMENT_H

/*
 *	Segment Object
 *	
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "common/omCommon.h"
#include "system/omManageableObject.h"
#include "volume/omMipChunkCoord.h"
#include "utility/omHdf5Path.h"

class OmSegmentCache;

class OmSegment {

public:
	OmSegment(OmId segmentID, SEGMENT_DATA_TYPE value, OmSegmentCache * cache);
	OmSegment(OmSegmentCache * cache);

	void updateChunkCoordInfo( const OmMipChunkCoord & mipCoord );
	QSet< OmMipChunkCoord > & getChunks();

	void splitChildLowestThreshold();

	//accessors
	const Vector3<float>& GetColor();
	void SetColor(const Vector3<float> &);
	
	//drawing
	void ApplyColor(const OmBitfield &drawOps);

	SEGMENT_DATA_TYPE getValue();
	SegmentDataSet getValues();
	OmIds getIDs();

	OmId GetId();
	QString GetNote();
	void SetNote(QString);
	QString GetName();
	void SetName(QString);
	bool IsSelected();
	void SetSelected(bool isSelected);
	bool IsEnabled();
	void SetEnabled(bool);

	void Join(OmSegment *, double threshold = 0);
	void setParent(OmSegment * segment, double threshold);
	void clearParent();
	OmId getParent();
	void removeChild( OmSegment * segment );

	OmId getSegmentationID();
	double getThreshold();

private:
	OmId mID;
	SEGMENT_DATA_TYPE mValue;
	OmSegmentCache * mCache;

	Vector3<float> mColor;

	OmIds segmentsJoinedIntoMe;
	OmId parentSegID;
	double mThreshold;
	
	QSet< OmMipChunkCoord > chunks;
		
	void SetInitialColor();

	friend class OmSegmentCacheImpl;

	friend QDataStream &operator<<(QDataStream & out, const OmSegment & segment );
	friend QDataStream &operator>>(QDataStream & in, OmSegment & segment );
};

#endif
