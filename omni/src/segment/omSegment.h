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
	OmSegment(SEGMENT_DATA_TYPE value, OmSegmentCache * cache);
	OmSegment(OmSegmentCache * cache);

	void splitChildLowestThreshold();
	void splitTwoChildren(OmSegment * seg);

	//accessors
	OmColor GetColorInt(){ return mColorInt; }
	Vector3<float> GetColorFloat(){
		Vector3<float> color;
		color.x = mColorInt.red / 255.;
		color.y = mColorInt.green / 255.;
		color.z = mColorInt.blue / 255.;
		return color;
	}
	void SetColor(const Vector3<float> &);
	
	//drawing
	void ApplyColor(const OmBitfield &drawOps);

	SEGMENT_DATA_TYPE getValue();

	QString GetNote();
	void SetNote(QString);
	QString GetName();
	void SetName(QString);
	bool IsSelected();
	void SetSelected(bool isSelected);
	bool IsEnabled();
	void SetEnabled(bool);

	void setParent(OmSegment * segment, float threshold);
	OmSegment * getParent();

	OmId getSegmentationID();
	float getThreshold();

private:

	SEGMENT_DATA_TYPE mValue;
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
