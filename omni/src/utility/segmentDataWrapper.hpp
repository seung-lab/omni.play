#ifndef SEGMENT_DATA_WRAPPER_HPP
#define SEGMENT_DATA_WRAPPER_HPP

#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "volume/omSegmentation.h"

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
		, mSegmentationID(seg->getSegmentationID())
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

	inline bool isValidWrapper() const {
		if(!mID && !mSegmentationID){
			return false;
		}

		return OmProject::IsSegmentationValid(mSegmentationID) &&
			getSegmentCache()->IsSegmentValid(mID);
	}

	inline QString getName() const {
		return getSegment()->GetName();
	}

	inline QString getSegmentationName() const {
		return QString::fromStdString(getSegmentation().GetName());
	}

	inline bool isSelected() const {
		return getSegmentCache()->IsSegmentSelected(mID);
	}

	inline void setSelected(const bool isSelected,
			 const bool addToRecentList) const {
		getSegmentCache()->setSegmentSelected(mID, isSelected, addToRecentList);
	}

	inline bool isEnabled() const {
		return getSegmentCache()->isSegmentEnabled(mID);
	}

	inline void setEnabled(const bool enabled) const {
		getSegmentCache()->setSegmentEnabled(mID, enabled );
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

	inline OmSegmentation& getSegmentation() const {
		return OmProject::GetSegmentation(mSegmentationID);
	}

	inline OmSegment* getSegment() const {
		return getSegmentation().GetSegmentCache()->GetSegment( mID );
	}

	inline OmSegmentCache* getSegmentCache() const {
		return getSegmentation().GetSegmentCache();
	}

	inline uint64_t getSize() const {
		return getSegment()->size();
	}

	inline uint64_t getSizeWithChildren() const {
		return getSegment()->getSizeWithChildren();
	}

	inline OmID getSegmentationID() const {
		return mSegmentationID;
	}

	inline OmSegID getID() const {
		return mID;
	}

	inline OmSegID GetVoxelValue(const DataCoord& dataClickPoint) const {
		return getSegmentation().GetVoxelValue(dataClickPoint);
	}

	inline OmSegID FindRootID() const {
		return getSegmentCache()->findRootID(mID);
	}

	inline OmSegment* FindRoot() const {
		return getSegmentCache()->findRoot(mID);
	}

	inline void RandomizeColor() const {
		getSegment()->reRandomizeColor();
	}
};

#endif
