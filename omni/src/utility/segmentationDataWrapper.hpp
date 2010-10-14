#ifndef SEGMENTATION_DATA_WRAPPER_HPP
#define SEGMENTATION_DATA_WRAPPER_HPP

class SegmentationDataWrapper {
private:
	OmId mID;
	ObjectType mType;

public:
	SegmentationDataWrapper(){}

	explicit SegmentationDataWrapper(const OmId ID)
		: mID(ID)
		, mType(SEGMENTATION)
	{}

	explicit SegmentationDataWrapper( OmSegment * seg )
		: mID(seg->getSegmentationID())
		, mType(SEGMENTATION)
	{}

	OmId getSegmentationID() const {
		return mID;
	}
	OmId getID() const {
		return mID;
	}
	ObjectType getType() const {
		return mType;
	}

	bool isValid() const {
		return OmProject::IsSegmentationValid(mID);
	}

	OmSegmentation & getSegmentation() const {
		return OmProject::GetSegmentation(mID);
	}

	QString getName() const {
		return QString::fromStdString(OmProject::GetSegmentation(mID).GetName());
	}

	bool isEnabled() const {
		return OmProject::IsSegmentationEnabled(mID);
	}

	QString getNote() const {
		return OmProject::GetSegmentation(mID).GetNote();
	}

	unsigned int getNumberOfSegments() const {
		return OmProject::GetSegmentation(mID).GetSegmentCache()->GetNumSegments();
	}

	unsigned int getNumberOfTopSegments() const {
		return OmProject::GetSegmentation(mID).GetSegmentCache()->GetNumTopSegments();
	}

	OmSegmentCache* getSegmentCache() const {
		return OmProject::GetSegmentation(mID).GetSegmentCache();
	}

	quint32 getMaxSegmentValue() const {
		return OmProject::GetSegmentation(mID).GetSegmentCache()->getMaxValue();
	}

	quint64 getSegmentListSize(OmSegIDRootType type) const {
		return OmProject::GetSegmentation(mID).GetSegmentCache()->getSegmentListSize(type);
	}
};
#endif
