#ifndef SEGMENTATION_DATA_WRAPPER_HPP
#define SEGMENTATION_DATA_WRAPPER_HPP

class OmSegmentLists;

class SegmentationDataWrapper {
private:
	OmID mID;

public:
	SegmentationDataWrapper()
		: mID(0)
	{}

	explicit SegmentationDataWrapper(const OmID ID)
		: mID(ID)
	{}

	explicit SegmentationDataWrapper( OmSegment * seg )
		: mID(seg->GetSegmentationID())
	{}

	inline OmID GetSegmentationID() const {
		return mID;
	}

	inline OmID getID() const {
		return mID;
	}

	inline ObjectType getType() const {
		return SEGMENTATION;
	}

	inline bool IsValidWrapper() const
	{
		if(!mID){
			return false;
		}

		return OmProject::IsSegmentationValid(mID);
	}

	inline OmSegmentation& GetSegmentation() const {
		return OmProject::GetSegmentation(mID);
	}

	inline QString getName() const {
		return QString::fromStdString(GetSegmentation().GetName());
	}

	inline bool isEnabled() const {
		return OmProject::IsSegmentationEnabled(mID);
	}

	inline QString getNote() const {
		return GetSegmentation().GetNote();
	}

	inline uint32_t getNumberOfSegments() const {
		return GetSegmentation().GetSegmentCache()->GetNumSegments();
	}

	inline uint32_t getNumberOfTopSegments() const {
		return GetSegmentation().GetSegmentCache()->GetNumTopSegments();
	}

	inline OmSegmentCache* GetSegmentCache() const {
		return GetSegmentation().GetSegmentCache();
	}

	inline boost::shared_ptr<OmMST> getMST() const {
		return GetSegmentation().getMST();
	}

	inline uint32_t getMaxSegmentValue() const {
		return GetSegmentation().GetSegmentCache()->getMaxValue();
	}

	inline boost::shared_ptr<OmSegmentLists> GetSegmentLists() const {
		return GetSegmentation().GetSegmentLists();
	}

	inline boost::shared_ptr<OmMST> GetMST() const {
		return GetSegmentation().getMST();
	}

};
#endif
