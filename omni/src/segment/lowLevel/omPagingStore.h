#ifndef OM_PAGING_STORE_H
#define OM_PAGING_STORE_H

#include "common/omCommon.h"

#include <QSet>

// TODO: this was done as proof-of-concept; not sure how much slower 
//  struct constructor is compared to simple int POD... (purcaro)
BOOST_STRONG_TYPEDEF(quint32, PageNum )

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
	void SaveDirtySegmentPages();
	void AddToDirtyList( const OmSegID );
	void FlushDirtyItems();
	void SetBatchMode( const bool );

	T* GetSegmentFromValue(const OmSegID value );

 private:
	OmSegmentation * mSegmentation;
	OmSegmentCache * mParentCache;

	quint32 mPageSize;
	boost::unordered_map< PageNum, std::vector<T*> > mValueToSegPtrHash;
	QSet< PageNum > validPageNumbers;
	QSet< PageNum > loadedPageNumbers;
	QSet< PageNum > dirtySegmentPages;
	PageNum getValuePageNum( const OmSegID value );
	void LoadValuePage( const PageNum valuePageNum );
	void doSaveSegmentPage( const PageNum segPageNum );
	bool mAllPagesLoaded;

	bool amInBatchMode;
	bool needToFlush;

	template <class T2> friend QDataStream &operator<< (QDataStream & out, const OmPagingStore<T2> & ps );
	template <class T2> friend QDataStream &operator>> (QDataStream & in, OmPagingStore<T2> & ps );
};

#endif
