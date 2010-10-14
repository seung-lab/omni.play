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
	OmSegment()
		: value_(0)
		, parentSegID_(0)
		, threshold_(0)
		, immutable_(false)
		, size_(0)
		, edgeNumber_(-1)
		, freshnessForMeshes_(0)
	{}


	OmSegment(const OmSegID value,
			  OmSegmentCache* cache)
		: value_(value)
		, cache_(cache)
		, parentSegID_(0)
		, immutable_(false)
		, size_(0)
		, edgeNumber_(-1)
		, freshnessForMeshes_(0)
	{}

	inline OmSegID value(){
		return value_;
	}

	// color
	void RandomizeColor();
	void reRandomizeColor();
	OmColor GetColorInt(){ return colorInt_; }
	Vector3f GetColorFloat()
	{
		return Vector3f( colorInt_.red   / 255.,
						 colorInt_.green / 255.,
						 colorInt_.blue  / 255. );
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

	quint64 getSize() const { return size_; }
	void addToSize(const quint64 inc){
		zi::spinlock::pool<segment_size_mutex_pool_tag>::guard g(value_);
		size_ += inc;
	}

	quint64 getSizeWithChildren();

	bool GetImmutable() const { return immutable_; }
	void SetImmutable( const bool immutable);

	OmSegID getParentSegID() const { return parentSegID_; }
	void setParentSegID(const OmSegID val){ parentSegID_ = val; }
	OmSegID getRootSegID();
	void setParent(OmSegment * segment, const float);

	OmId getSegmentationID();

	float getThreshold() const { return threshold_; }
	void setThreshold(const float thres){ threshold_ = thres; }

	const DataBbox& getBounds() const {	return bounds_;	}
	void addToBounds(const DataBbox& box){
		zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(value_);
		bounds_.merge(box);
	}

	uint32_t getFreshnessForMeshes() const {return freshnessForMeshes_;}
	void touchFreshnessForMeshes(){++freshnessForMeshes_;}

	const std::set<OmSegment*>& getChildren() const {
		return segmentsJoinedIntoMe_;
	}
	void addChild(OmSegment* child){
		segmentsJoinedIntoMe_.insert(child);
	}
	void removeChild(OmSegment* child){
		segmentsJoinedIntoMe_.erase(child);
	}

	int getEdgeNumber() const { return edgeNumber_; }
	void setEdgeNumber(const int num){ edgeNumber_ = num; }

	const OmSegmentEdge& getCustomMergeEdge() const {return customMergeEdge_;}
	void setCustomMergeEdge(const OmSegmentEdge& e){customMergeEdge_=e;}

	OmSegmentCache* getSegmentCache(){ return cache_; }

private:
	OmSegID value_;
	OmSegmentCache* cache_;
	OmSegID parentSegID_;
	OmColor colorInt_;
	std::set<OmSegment*> segmentsJoinedIntoMe_;
	float threshold_;
	bool immutable_;
	quint64 size_;

	int edgeNumber_; // index of edge in main edge list
	OmSegmentEdge customMergeEdge_;
	uint32_t freshnessForMeshes_;
	DataBbox bounds_;

	struct segment_bounds_mutex_pool_tag;
	struct segment_size_mutex_pool_tag;

	friend class OmDataArchiveSegment;
};

#endif
