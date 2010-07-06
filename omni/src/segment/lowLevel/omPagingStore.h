#ifndef OM_PAGING_STORE_H
#define OM_PAGING_STORE_H

#include "common/omCommon.h"

#include <QSet>

typedef quint32 PageNum;

class OmSegmentation;
class OmSegmentCache;

template < class T >
class OmPagingStore {
 public:
	OmPagingStore( OmSegmentation *, OmSegmentCache *);
	~OmPagingStore();

	quint32 getPageSize() { return mPageSize; }
	void SetSegmentationID( const OmId );

	void AddItem( T* item );
	bool IsValueAlreadyMapped( const OmSegID );

	void SaveAllLoadedPages();
	void SaveDirtyPages();
	void AddToDirtyList( const OmSegID );
	void FlushDirtyItems();
	void SetBatchMode( const bool );

	T* GetItemFromValue(const OmSegID value );

 private:
	OmSegmentation *const mSegmentation;
	OmSegmentCache *const mParentCache;

	quint32 mPageSize;
	std::vector< std::vector<T*> > mValueToSegPtr;
	QSet< PageNum > validPageNumbers;
	QSet< PageNum > loadedPageNumbers;
	QSet< PageNum > dirtyPages;
	bool mAllPagesLoaded;

	bool amInBatchMode;
	bool needToFlush;

	PageNum getValuePageNum( const OmSegID value );
	void resizeVectorIfNeeded(const PageNum pageNum );
	void LoadValuePage( const PageNum valuePageNum );
	void doSavePage( const PageNum segPageNum );

	template <class T2> friend QDataStream &operator<< (QDataStream & out, const OmPagingStore<T2> & ps );
	template <class T2> friend QDataStream &operator>> (QDataStream & in, OmPagingStore<T2> & ps );
};

#endif
