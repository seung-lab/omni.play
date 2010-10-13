#ifndef OM_SEGMENT_H
#define OM_SEGMENT_H

/*
 *	Segment Object
 *
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "common/omCommon.h"
#include "segment/omSegmentEdge.h"
#include "zi/omMutex.h"

class OmSegmentCache;

class OmSegment {
public:
	OmSegment(const OmSegID, boost::shared_ptr<OmSegmentCache>);
	const OmSegID value;

	// color
	void RandomizeColor();
	void reRandomizeColor();
	OmColor GetColorInt(){ return mColorInt; }
	Vector3f GetColorFloat()
	{
		return Vector3f( mColorInt.red   / 255.,
						 mColorInt.green / 255.,
						 mColorInt.blue  / 255. );
	}
	void SetColor(const Vector3f &);

	QString GetNote();
	void SetNote(const QString &);
	QString GetName();
	void SetName(const QString &);
	bool IsSelected();
	void SetSelected(const bool, const bool);
	bool IsEnabled();
	void SetEnabled( const bool);

	quint64 getSize() const { return mSize; }
	void addToSize(const quint64 inc){
		zi::spinlock::pool<segment_size_mutex_pool_tag>::guard g(value);
		mSize += inc;
	}

	quint64 getSizeWithChildren();

	bool GetImmutable() const { return mImmutable; }
	void SetImmutable( const bool immutable);

	OmSegID getParentSegID() const { return mParentSegID; }
	void setParentSegID(const OmSegID val){ mParentSegID = val; }
	OmSegID getRootSegID();
	void setParent(OmSegment * segment, const float);

	OmId getSegmentationID();

	float getThreshold() const { return mThreshold; }
	void setThreshold(const float thres){ mThreshold = thres; }

	const DataBbox& getBounds() const {	return mBounds;	}
	void addToBounds(const DataBbox& box){
		zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(value);
		mBounds.merge(box);
	}

	uint32_t getFreshnessForMeshes() const {return mFreshnessForMeshes;}
	void touchFreshnessForMeshes(){++mFreshnessForMeshes;}

	const std::set<OmSegment*>& getChildren() const {
		return segmentsJoinedIntoMe;
	}
	void addChild(OmSegment* child){
		segmentsJoinedIntoMe.insert(child);
	}
	void removeChild(OmSegment* child){
		segmentsJoinedIntoMe.erase(child);
	}

	int getEdgeNumber() const { return mEdgeNumber; }
	void setEdgeNumber(const int num){ mEdgeNumber = num; }

	const OmSegmentEdge& getCustomMergeEdge() const {return mCustomMergeEdge;}
	void setCustomMergeEdge(const OmSegmentEdge& e){mCustomMergeEdge=e;}

	boost::shared_ptr<OmSegmentCache> getSegmentCache(){ return mCache; }

private:
	boost::shared_ptr<OmSegmentCache> mCache;
	OmColor mColorInt;
	std::set<OmSegment*> segmentsJoinedIntoMe;
	OmSegID mParentSegID;
	float mThreshold;
	bool mImmutable;
	quint64 mSize;

	int mEdgeNumber; // index of edge in main edge list
	OmSegmentEdge mCustomMergeEdge;

	uint32_t mFreshnessForMeshes;

	DataBbox mBounds;

	struct segment_bounds_mutex_pool_tag;
	struct segment_size_mutex_pool_tag;

	friend class OmDataArchiveSegment;
};

#endif
