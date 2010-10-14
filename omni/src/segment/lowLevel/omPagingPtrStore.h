#ifndef OM_PAGING_PTR_STORE_H
#define OM_PAGING_PTR_STORE_H

#include "common/om.hpp"
#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "segment/lowLevel/omSegmentPage.hpp"

#include <QSet>

typedef quint32 PageNum;

class OmDataPath;
class OmSegmentation;
class OmSegmentCache;

template <typename T>
class OmPagingPtrStore {
 public:
	OmPagingPtrStore(OmSegmentation*);
	virtual ~OmPagingPtrStore(){}

	quint32 getPageSize() { return mPageSize; }
	void SetSegmentationID(const OmId);

	void AddItem(T* item);
	bool IsValueAlreadyMapped(const OmSegID);

	void SaveAllLoadedPages();
	void SaveDirtyPages();
	void AddToDirtyList(const OmSegID);
	void FlushDirtyItems();
	void SetBatchMode(const bool);

	T* GetItemFromValue(const OmSegID value);

	void UpgradeSegmentSerialization();

 private:
	OmSegmentation *const mSegmentation;

	uint32_t mPageSize;
	std::vector<OmSegmentPage> mValueToSeg;
	QSet<PageNum> validPageNumbers;
	QSet<PageNum> loadedPageNumbers;
	QSet<PageNum> dirtyPages;
	bool mAllPagesLoaded;

	bool amInBatchMode;
	bool needToFlush;

	inline PageNum getValuePageNum(const OmSegID value){
		return PageNum(value / mPageSize);
	}

	void resizeVectorIfNeeded(const PageNum pageNum);
	void loadValuePage(const PageNum);
	void loadValuePage(const PageNum, const om::RewriteSegments);
	void doSavePage(const PageNum segPageNum);
	OmDataPath getPath(const PageNum pageNum);

	template <class T2> friend QDataStream &operator<< (QDataStream & out, const OmPagingPtrStore<T2> & ps );
	template <class T2> friend QDataStream &operator>> (QDataStream & in, OmPagingPtrStore<T2> & ps );
};

#endif
