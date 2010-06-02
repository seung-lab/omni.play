#ifndef OM_SEGMENT_H
#define OM_SEGMENT_H

/*
 *	Segment Object
 *	
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "common/omCommon.h"
#include "segment/omSegmentQueue.h"
#include "segment/omSegmentColorizer.h"

class OmSegmentCache;
class OmMipChunkCoord;
class OmViewGroupState;

class OmSegment {

public:
	OmSegment(const OmSegID & value, OmSegmentCache * cache);
	OmSegment(OmSegmentCache * cache);

	void splitChildLowestThreshold();
	void splitTwoChildren(OmSegment * seg);

	//accessors
	const OmColor & GetColorInt(){ return mColorInt; }
	Vector3<float> GetColorFloat(){
		return 	Vector3<float>( mColorInt.red / 255.,
					mColorInt.green / 255.,
					mColorInt.blue / 255. );
	}
	void SetColor(const Vector3<float> &);
	
	const OmSegID & getValue();

	QString GetNote();
	void SetNote(const QString &);
	QString GetName();
	void SetName(const QString &);
	bool IsSelected();
	void SetSelected(const bool);
	bool IsEnabled();
	void SetEnabled( const bool);

	quint64 getSize(){ return mSize; }

	void SetImmutable(bool immutable);
	bool GetImmutable();

	OmSegID getParentSegID(){ return mParentSegID; }
	void setParent(OmSegment * segment, const float);

	OmId getSegmentationID();
	float getThreshold();

private:
	OmSegID mValue;
	OmSegmentCache * mCache;

	OmColor mColorInt;

	OmSegIDsSet segmentsJoinedIntoMe;
	OmSegmentQueue queue;

	OmId mParentSegID;
	float mThreshold;
	bool mImmutable;

	quint64 mSize;
	quint64 mSizeOfAllChildren; // excludes mSize

	void SetInitialColor();

	friend class OmSegmentCacheImpl;
	friend class OmSegmentColorizer;
	friend class OmDataArchiveSegment;
	friend class OmSegmentIterator;
};

#endif
