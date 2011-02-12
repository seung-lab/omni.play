#ifndef OM_PAGING_PTR_STORE_H
#define OM_PAGING_PTR_STORE_H

#include "common/om.hpp"
#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "segment/io/omSegmentPage.hpp"

#include <QSet>

class OmDataPath;
class OmSegmentation;
class OmSegmentCache;

class OmPagingPtrStore {
 public:
	OmPagingPtrStore(OmSegmentation*);
	virtual ~OmPagingPtrStore(){}

	void Flush();

	quint32 getPageSize() {
		return pageSize_;
	}

	OmSegment* AddSegment(const OmSegID value);
	OmSegment* GetSegment(const OmSegID value);

 private:
	OmSegmentation *const segmentation_;

	uint32_t pageSize_;
	std::vector<OmSegmentPage> pages_;
	QSet<PageNum> validPageNumbers_;

	inline PageNum getValuePageNum(const OmSegID value){
		return PageNum(value / pageSize_);
	}

	void loadAllSegmentPages();
	void resizeVectorIfNeeded(const PageNum pageNum);

	QString metadataPathQStr();
	void loadMetadata();
	void storeMetadata();

	friend QDataStream &operator<< (QDataStream& out, const OmPagingPtrStore&);
	friend QDataStream &operator>> (QDataStream& in, OmPagingPtrStore&);
};

#endif
