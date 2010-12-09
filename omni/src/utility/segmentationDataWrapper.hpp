#ifndef SEGMENTATION_DATA_WRAPPER_HPP
#define SEGMENTATION_DATA_WRAPPER_HPP

#include "segment/omSegment.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentCache.h"

class OmMST;
class OmValidGroupNum;
class OmSegmentLists;

class SegmentationDataWrapper {
private:
	OmID id_;
	mutable boost::optional<OmSegmentation&> segmentation_;

public:
	SegmentationDataWrapper()
		: id_(0)
	{}

	explicit SegmentationDataWrapper(const OmID ID)
		: id_(ID)
	{}

	SegmentationDataWrapper(const SegmentationDataWrapper& sdw)
		: id_(sdw.id_)
	{}

	explicit SegmentationDataWrapper(OmSegment* seg)
		: id_(seg->GetSegmentationID())
	{}

	inline void set(const SegmentationDataWrapper& sdw)
	{
		id_ = sdw.id_;
	}

	SegmentationDataWrapper& operator =(const SegmentationDataWrapper& sdw)
	{
		if (this != &sdw){
			id_ = sdw.id_;
			segmentation_ = sdw.segmentation_;
		}
		return *this;
	}

	bool operator ==(const SegmentationDataWrapper& sdw) const {
		return id_ == sdw.id_;
	}

	bool operator !=(const SegmentationDataWrapper& sdw) const {
		return !(*this == sdw);
	}

	inline OmID GetSegmentationID() const {
		return id_;
	}

	inline OmID getID() const {
		return id_;
	}

	OmSegmentation& Create()
	{
		OmSegmentation& s = OmProject::AddSegmentation();
		id_ = s.GetID();
		printf("create segmentation %d\n", id_);
		segmentation_ =	boost::optional<OmSegmentation&>(s);
		return s;
	}

	inline ObjectType getType() const {
		return SEGMENTATION;
	}

	inline bool IsSegmentationValid() const
	{
		if(!id_){
			return false;
		}

		return OmProject::IsSegmentationValid(id_);
	}

	inline OmSegmentation& GetSegmentation() const
	{
		if(!segmentation_){
			//printf("cached segmentation...\n");
			segmentation_ =
				boost::optional<OmSegmentation&>(OmProject::GetSegmentation(id_));
		}
		return *segmentation_;
	}

	inline OmSegmentation* GetSegmentationPtr() const
	{
		OmSegmentation& seg = GetSegmentation();
		return &seg;
	}

	inline QString getName() const {
		return QString::fromStdString(GetSegmentation().GetName());
	}

	inline bool isEnabled() const {
		return OmProject::IsSegmentationEnabled(id_);
	}

	inline QString getNote() const {
		return GetSegmentation().GetNote();
	}

	inline uint32_t getNumberOfSegments() const {
		return GetSegmentCache()->GetNumSegments();
	}

	inline uint32_t getNumberOfTopSegments() const {
		return GetSegmentCache()->GetNumTopSegments();
	}

	inline OmSegmentCache* GetSegmentCache() const {
		return GetSegmentation().GetSegmentCache();
	}

	inline boost::shared_ptr<OmMST> getMST() const {
		return GetSegmentation().getMST();
	}

	inline uint32_t getMaxSegmentValue() const {
		return GetSegmentCache()->getMaxValue();
	}

	inline boost::shared_ptr<OmSegmentLists> GetSegmentLists() const {
		return GetSegmentation().GetSegmentLists();
	}

	inline boost::shared_ptr<OmMST> GetMST() const {
		return GetSegmentation().getMST();
	}

	inline boost::shared_ptr<OmValidGroupNum>& GetValidGroupNum() const {
		return GetSegmentation().GetValidGroupNum();
	}

	inline const Vector3f& GetDataResolution() const {
		return GetSegmentation().GetDataResolution();
	}

	inline const OmSegIDsSet& GetSelectedSegmentIds() const {
		return GetSegmentCache()->GetSelectedSegmentIds();
	}
};
#endif
