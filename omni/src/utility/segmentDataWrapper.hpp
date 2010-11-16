#ifndef SEGMENT_DATA_WRAPPER_HPP
#define SEGMENT_DATA_WRAPPER_HPP

#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "volume/omSegmentation.h"
#include "utility/segmentationDataWrapper.hpp"

class SegmentDataWrapper {
private:
	OmSegID mID;
	OmID mSegmentationID;

public:
	SegmentDataWrapper()
		: mID(0)
		, mSegmentationID(0)
	{}

	SegmentDataWrapper(const OmID segmentationID,
					   const OmSegID segmentID)
		: mID(segmentID)
		, mSegmentationID(segmentationID)
	{}

	explicit SegmentDataWrapper(OmSegment* seg)
		: mID(seg->value())
		, mSegmentationID(seg->GetSegmentationID())
	{}

	inline void set(const SegmentDataWrapper& sdw)
	{
		mID = sdw.mID;
		mSegmentationID = sdw.mSegmentationID;
	}

	SegmentDataWrapper& operator =(const SegmentDataWrapper& sdw){
		if (this != &sdw){
			mID = sdw.mID;
			mSegmentationID = sdw.mSegmentationID;
		}
		return *this;
	}

	bool operator ==(const SegmentDataWrapper& sdw) const {
		return mID == sdw.mID && mSegmentationID == sdw.mSegmentationID;
	}

	bool operator !=(const SegmentDataWrapper& sdw) const {
		return !(*this == sdw);
	}

	SegmentationDataWrapper MakeSegmentationDataWrapper() const {
		return SegmentationDataWrapper(mSegmentationID);
	}

	inline bool isValidWrapper() const {
		if(!mID && !mSegmentationID){
			return false;
		}

		return OmProject::IsSegmentationValid(mSegmentationID) &&
			GetSegmentCache()->IsSegmentValid(mID);
	}

	inline QString getName() const {
		return getSegment()->GetName();
	}

	inline QString GetSegmentationName() const {
		return QString::fromStdString(GetSegmentation().GetName());
	}

	inline bool isSelected() const {
		return GetSegmentCache()->IsSegmentSelected(mID);
	}

	inline void setSelected(const bool isSelected,
			 const bool addToRecentList) const {
		GetSegmentCache()->setSegmentSelected(mID, isSelected, addToRecentList);
	}

	inline bool isEnabled() const {
		return GetSegmentCache()->isSegmentEnabled(mID);
	}

	inline void setEnabled(const bool enabled) const {
		GetSegmentCache()->setSegmentEnabled(mID, enabled );
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
		return OmProject::GetSegmentation(mSegmentationID);
	}

	inline OmSegment* getSegment() const {
		return GetSegmentCache()->GetSegment( mID );
	}

	inline OmSegmentCache* GetSegmentCache() const {
		return GetSegmentation().GetSegmentCache();
	}

	inline uint64_t getSize() const {
		return getSegment()->size();
	}

	inline uint64_t getSizeWithChildren() const {
		return getSegment()->getSizeWithChildren();
	}

	inline OmID GetSegmentationID() const {
		return mSegmentationID;
	}

	inline OmSegID getID() const {
		return mID;
	}

	inline OmSegID GetVoxelValue(const DataCoord& dataClickPoint) const {
		return GetSegmentation().GetVoxelValue(dataClickPoint);
	}

	inline OmSegID FindRootID() const {
		return GetSegmentCache()->findRootID(mID);
	}

	inline OmSegment* FindRoot() const {
		return GetSegmentCache()->findRoot(mID);
	}

	inline void RandomizeColor() const {
		getSegment()->reRandomizeColor();
	}
};

#endif
