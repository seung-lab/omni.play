#ifndef OM_SEGMENT_PAGE_HPP
#define OM_SEGMENT_PAGE_HPP

#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "utility/omSmartPtr.hpp"
#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "utility/dataWrappers.h"

class OmSegmentPage {
private:
	OmSegmentation* segmentation_;
	PageNum pageNum_;
	uint32_t pageSize_;

	boost::shared_ptr<OmSegment> segments_;
	OmSegment* segmentsRawPtr_;

public:
	OmSegmentPage()
		: pageNum_(0)
		, pageSize_(0)
		, segmentsRawPtr_(NULL)
	{}

	OmSegmentPage(OmSegmentation* segmentation, const PageNum pageNum,
				  const uint32_t pageSize)
		: segmentation_(segmentation)
		, pageNum_(pageNum)
		, pageSize_(pageSize)
	{
		segments_ = OmSmartPtr<OmSegment>::MallocNumElements(pageSize_,
															 om::ZERO_FILL);
		segmentsRawPtr_ = segments_.get();
	}

	void Create()
	{}

	void Load(const om::RewriteSegments rewriteSegments)
	{
		OmDataArchiveSegment::ArchiveRead(getPath(),
										  this,
										  segmentation_->GetSegmentCache(),
										  rewriteSegments);
	}

	void Save()
	{
		OmDataArchiveSegment::ArchiveWrite(getPath(),
										   this,
										   segmentation_->GetSegmentCache());
	}

	inline OmSegment& operator[](const uint32_t index){
		assert(pageSize_ && index < pageSize_);
		return segmentsRawPtr_[index];
	}

	inline const OmSegment& operator[](const uint32_t index) const {
		assert(pageSize_ && index < pageSize_);
		return segmentsRawPtr_[index];
	}

private:
	OmDataPath getPath(){
		return OmDataPaths::getSegmentPagePath(segmentation_->GetID(),
											   pageNum_);
	}
};

#endif
