#ifndef OM_SEGMENT_H
#define OM_SEGMENT_H

/*
 *	Segment Object
 *
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "common/omCommon.h"
#include "segment/omSegmentEdge.h"

class OmSegmentCache;

class OmSegment {

public:
	OmSegment( const OmSegID value, OmSegmentCache * cache);
	OmSegment(OmSegmentCache * cache);

	OmSegID getValue(){ return mValue; }

	// color
	void reRandomizeColor();
	OmColor GetColorInt(){ return mColorInt; }
	inline Vector3f GetColorFloat(){
		return Vector3f( mColorInt.red   / 255.,
				 mColorInt.green / 255.,
				 mColorInt.blue  / 255. ); }
	void SetColor(const Vector3f &);

	QString GetNote();
	void SetNote(const QString &);
	QString GetName();
	void SetName(const QString &);
	bool IsSelected();
	void SetSelected(const bool, const bool);
	bool IsEnabled();
	void SetEnabled( const bool);

	quint64 getSize(){ return mSize; }
	quint64 getSizeWithChildren();

	void SetImmutable( const bool immutable);
	bool GetImmutable(){ return mImmutable; }

	OmSegID getParentSegID(){ return mParentSegID; }
	OmSegID getRootSegID();
	void setParent(OmSegment * segment, const float);

	OmId getSegmentationID();
	float getThreshold(){ return mThreshold; }
	DataBbox & getBounds() { return mBounds; }

	quint32 getFreshnessForMeshes(){return mFreshnessForMeshes;}

private:
	DataBbox mBounds;
	OmSegID mValue;
	OmSegmentCache * mCache;

	OmColor mColorInt;

	OmSegIDsSet segmentsJoinedIntoMe;

	OmId mParentSegID;
	float mThreshold;
	bool mImmutable;

	quint64 mSize;

	int mEdgeNumber; // index of edge in main edge list
	OmSegmentEdge mCustomMergeEdge;

	void SetInitialColor();

	quint32 mFreshnessForMeshes;

	friend class OmSegmentListBySize;
	friend class OmSegmentCacheImpl;
	friend class OmSegmentCacheImplLowLevel;
	friend class OmSegmentColorizer;
	friend class OmDataArchiveSegment;
	friend class OmSegmentIterator;
	friend class OmSegmentIteratorLowLevel;
	friend class OmSegmentGraph;
};

#endif
