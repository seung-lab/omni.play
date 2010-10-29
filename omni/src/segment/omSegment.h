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

struct OmSegmentDataV2 {
	OmSegID value;
	OmColor color;
	bool immutable;
	uint64_t size;
	DataBbox bounds;
};

struct OmSegmentDataV3 {
	OmSegID value;
	OmColor color;
	om::OmSegListType listType;
	uint64_t size;
	DataBbox bounds;
};

class OmSegment {
public:
	OmSegment()
		: data_(NULL)
		, cache_(NULL)
		, parentSegID_(0)
		, threshold_(0)
		, edgeNumber_(-1)
		, freshnessForMeshes_(0)
	{}

	inline OmSegID value() const {
		return data_->value;
	}

	// color
	void RandomizeColor();
	void reRandomizeColor();
	OmColor GetColorInt(){ return data_->color; }
	Vector3f GetColorFloat() const
	{
		return Vector3f( data_->color.red   / 255.,
						 data_->color.green / 255.,
						 data_->color.blue  / 255. );
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

	uint64_t size() const { return data_->size; }
	void addToSize(const uint64_t inc){
		zi::spinlock::pool<segment_size_mutex_pool_tag>::guard g(data_->value);
		data_->size += inc;
	}

	uint64_t getSizeWithChildren();

	bool IsValid() const { return om::VALID == data_->listType; }
	om::OmSegListType GetListType() const { return data_->listType; }
	void SetListType(const om::OmSegListType type) { data_->listType = type; }

	OmSegID getParentSegID() const { return parentSegID_; }
	void setParentSegID(const OmSegID val){ parentSegID_ = val; }
	OmSegID getRootSegID();
	void setParent(OmSegment * segment, const double);

	OmID getSegmentationID();

	double getThreshold() const { return threshold_; }
	void setThreshold(const double thres){ threshold_ = thres; }

	const DataBbox& getBounds() const {	return data_->bounds; }
	void clearBounds() { data_->bounds = DataBbox(); }
	void addToBounds(const DataBbox& box){
		zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(data_->value);
		data_->bounds.merge(box);
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
	OmSegmentDataV3* data_;

	OmSegmentCache* cache_;
	OmSegID parentSegID_;
	std::set<OmSegment*> segmentsJoinedIntoMe_;
	double threshold_;

	int edgeNumber_; // index of edge in main edge list
	OmSegmentEdge customMergeEdge_;
	uint32_t freshnessForMeshes_;

	struct segment_bounds_mutex_pool_tag;
	struct segment_size_mutex_pool_tag;

	friend class OmPagingPtrStore;
	friend class OmSegmentPage;
	friend class OmDataArchiveSegment;
};

#endif
