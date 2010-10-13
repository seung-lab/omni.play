#ifndef SEGMENT_DATA_WRAPPER_HPP
#define SEGMENT_DATA_WRAPPER_HPP

#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "volume/omSegmentation.h"

class SegmentDataWrapper {
private:
	OmSegID mID;
	OmId mSegmentationID;

public:
	SegmentDataWrapper(){}

	SegmentDataWrapper(const OmId segmentationID,
			   const OmSegID segmentID)
		: mID(segmentID)
		, mSegmentationID( segmentationID )
	{}

	explicit SegmentDataWrapper(OmSegment* seg )
		: mID(seg->value())
		, mSegmentationID( seg->getSegmentationID() )
	{}

	void set(const SegmentDataWrapper& sdw)
	{
		mID = sdw.mID;
		mSegmentationID = sdw.mSegmentationID;
	}

	bool isValid() const {
		return OmProject::IsSegmentationValid(mSegmentationID) &&
			getSegmentCache()->IsSegmentValid(mID);
	}

	QString getName() const {
		return getSegment()->GetName();
	}

	QString getSegmentationName() const {
		return QString::fromStdString(getSegmentation().GetName());
	}

	bool isSelected() const {
		return getSegmentCache()->IsSegmentSelected(mID);
	}

	void setSelected(const bool isSelected,
			 const bool addToRecentList) const {
		getSegmentCache()->setSegmentSelected(mID, isSelected, addToRecentList);
	}

	bool isEnabled() const {
		return getSegmentCache()->isSegmentEnabled(mID);
	}

	void setEnabled(const bool enabled) const {
		getSegmentCache()->setSegmentEnabled(mID, enabled );
	}

	QString getNote() const {
		return getSegment()->GetNote();
	}

	void setNote(const QString& str) const {
		getSegment()->SetNote(str);
	}

	QString getIDstr() const {
		return QString("%1").arg(getID());
	}

	OmColor getColorInt() const {
		return getSegment()->GetColorInt();
	}

	Vector3f getColorFloat() const {
		return getSegment()->GetColorFloat();
	}

	void setColor(const Vector3f& color) const {
		getSegment()->SetColor( color );
	}

	void setName( const QString& str ) const {
		getSegment()->SetName( str );
	}

	OmSegmentation& getSegmentation() const {
		return OmProject::GetSegmentation(mSegmentationID);
	}

	OmSegment* getSegment() const {
		return getSegmentation().GetSegmentCache()->GetSegment( mID );
	}

	boost::shared_ptr<OmSegmentCache> getSegmentCache() const {
		return getSegmentation().GetSegmentCache();
	}

	quint64 getSize() const {
		return getSegment()->getSize();
	}

	quint64 getSizeWithChildren() const {
		return getSegment()->getSizeWithChildren();
	}

	OmId getSegmentationID() const {
		return mSegmentationID;
	}

	OmSegID getID() const {
		return mID;
	}

	OmSegID GetVoxelValue(const DataCoord& dataClickPoint) const {
		return getSegmentation().GetVoxelValue(dataClickPoint);
	}
};

#endif
