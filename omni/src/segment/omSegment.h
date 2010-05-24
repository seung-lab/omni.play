#ifndef OM_SEGMENT_H
#define OM_SEGMENT_H

/*
 *	Segment Object
 *	
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "common/omCommon.h"

class OmSegmentCache;
class OmMipChunkCoord;

class OmSegment {

public:
	OmSegment(const OmSegID & value, OmSegmentCache * cache);
	OmSegment(OmSegmentCache * cache);

	void splitChildLowestThreshold();
	void splitTwoChildren(OmSegment * seg);

	//accessors
	OmColor GetColorInt(){ return mColorInt; }
	Vector3<float> GetColorFloat(){
		return 	Vector3<float>( mColorInt.red / 255.,
					mColorInt.green / 255.,
					mColorInt.blue / 255. );
	}
	void SetColor(const Vector3<float> &);
	
	//drawing
	void ApplyColor(const OmBitfield &drawOps);

	const OmSegID & getValue();

	QString GetNote();
	void SetNote(QString);
	QString GetName();
	void SetName(QString);
	bool IsSelected();
	void SetSelected(bool isSelected);
	bool IsEnabled();
	void SetEnabled(bool);

	void setParent(OmSegment * segment, float threshold);

	OmId getSegmentationID();
	float getThreshold();

private:

	OmSegID mValue;
	OmSegmentCache * mCache;

	OmColor mColorInt;

	QList<OmId> segmentsJoinedIntoMe;
	OmId mParentSegID;
	float mThreshold;

	void SetInitialColor();

	friend class OmSegmentCacheImpl;
	friend class OmSegmentColorizer;
	friend class OmDataArchiveSegment;
	friend class OmSegmentIterator;
};

#endif
