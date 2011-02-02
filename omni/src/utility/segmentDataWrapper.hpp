#ifndef SEGMENT_DATA_WRAPPER_HPP
#define SEGMENT_DATA_WRAPPER_HPP

#include "project/omSegmentationManager.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "volume/omSegmentation.h"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegmentLists.hpp"

class SegmentDataWrapper {
private:
	OmSegID segmentID_;
	OmID segmentationID_;

	// not allowed--allows wrappers to be implicitly converted!!!
	explicit SegmentDataWrapper(const SegmentationDataWrapper& sdw);

public:
	SegmentDataWrapper()
		: segmentID_(0)
		, segmentationID_(0)
	{}

	SegmentDataWrapper(const OmID segmentationID,
					   const OmSegID segmentID)
		: segmentID_(segmentID)
		, segmentationID_(segmentationID)
	{}

	explicit SegmentDataWrapper(OmSegment* seg)
		: segmentID_(seg->value())
		, segmentationID_(seg->GetSegmentationID())
	{}

	SegmentDataWrapper(const SegmentationDataWrapper& sdw, const OmSegID segID)
		: segmentID_(segID)
		, segmentationID_(sdw.GetSegmentationID())
	{}

	inline void set(const SegmentDataWrapper& sdw)
	{
		segmentID_ = sdw.segmentID_;
		segmentationID_ = sdw.segmentationID_;
	}

	inline void SetSegmentID(const OmSegID segID){
		segmentID_ = segID;
	}

	SegmentDataWrapper& operator =(const SegmentDataWrapper& sdw){
		if (this != &sdw){
			segmentID_ = sdw.segmentID_;
			segmentationID_ = sdw.segmentationID_;
		}
		return *this;
	}

	inline OmSegID GetSegmentID() const {
		return segmentID_;
	}

	bool operator ==(const SegmentDataWrapper& sdw) const {
		return segmentID_ == sdw.segmentID_ && segmentationID_ == sdw.segmentationID_;
	}

	bool operator !=(const SegmentDataWrapper& sdw) const {
		return !(*this == sdw);
	}

	SegmentationDataWrapper MakeSegmentationDataWrapper() const {
		return SegmentationDataWrapper(segmentationID_);
	}

	inline bool IsSegmentationValid() const
	{
		if(!segmentationID_){
			return false;
		}

		return OmProject::Volumes().Segmentations().IsSegmentationValid(segmentationID_);
	}

	inline bool IsSegmentValid() const
	{
		if(!segmentID_ && !segmentationID_){
			return false;
		}

		return OmProject::Volumes().Segmentations().IsSegmentationValid(segmentationID_) &&
			SegmentCache()->IsSegmentValid(segmentID_);
	}

	inline QString getName() const {
		return getSegment()->GetName();
	}

	inline QString GetSegmentationName() const {
		return QString::fromStdString(GetSegmentation().GetName());
	}

	inline bool isSelected() const {
		return SegmentCache()->IsSegmentSelected(segmentID_);
	}

	inline void setSelected(const bool isSelected,
			 const bool addToRecentList) const {
		SegmentCache()->setSegmentSelected(segmentID_, isSelected, addToRecentList);
	}

	inline bool isEnabled() const {
		return SegmentCache()->isSegmentEnabled(segmentID_);
	}

	inline void setEnabled(const bool enabled) const {
		SegmentCache()->setSegmentEnabled(segmentID_, enabled );
	}

	inline QString getNote() const {
		return getSegment()->GetNote();
	}

	inline void setNote(const QString& str) const {
		getSegment()->SetNote(str);
	}

	inline QString getIDstr() const {
		return QString("%1").arg(getID());
	}

	inline OmColor getColorInt() const {
		return getSegment()->GetColorInt();
	}

	inline Vector3f getColorFloat() const {
		return getSegment()->GetColorFloat();
	}

	inline void setColor(const Vector3f& color) const {
		getSegment()->SetColor( color );
	}

	inline void setName( const QString& str ) const {
		getSegment()->SetName( str );
	}

	inline OmSegmentation& GetSegmentation() const {
		return OmProject::Volumes().Segmentations().GetSegmentation(segmentationID_);
	}

	inline OmSegmentLists* SegmentLists() const {
		return GetSegmentation().SegmentLists();
	}

	inline OmSegment* getSegment() const {
		return SegmentCache()->GetSegment( segmentID_ );
	}

	inline OmSegmentCache* SegmentCache() const {
		return GetSegmentation().SegmentCache();
	}

	inline uint64_t getSize() const {
		return getSegment()->size();
	}

	inline uint64_t getSizeWithChildren() const {
		return getSegment()->getSizeWithChildren();
	}

	inline OmID GetSegmentationID() const {
		return segmentationID_;
	}

	inline OmSegID getID() const {
		return segmentID_;
	}

	inline OmSegID GetVoxelValue(const DataCoord& dataClickPoint) const {
		return GetSegmentation().GetVoxelValue(dataClickPoint);
	}

	inline OmSegID FindRootID() const {
		return SegmentCache()->findRootID(segmentID_);
	}

	inline OmSegment* FindRoot() const {
		return SegmentCache()->findRoot(segmentID_);
	}

	inline void RandomizeColor() const {
		getSegment()->reRandomizeColor();
	}

	friend std::ostream& operator<<(std::ostream &out,
									const SegmentDataWrapper& s)
	{
		out << "(segmentation " << s.segmentationID_
			<< ", segment " << s.segmentID_ << ")";
		return out;
	}
};

#endif
