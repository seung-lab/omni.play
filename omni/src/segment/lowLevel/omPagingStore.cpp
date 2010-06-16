#include "segment/lowLevel/omPagingStore.h"
#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataPath.h"
#include "segment/omSegmentCache.h"
#include "volume/omSegmentation.h"

template< class T >
OmPagingStore<T>::OmPagingStore(OmSegmentation * segmentation, OmSegmentCache * cache)
	: mSegmentation(segmentation)
	, mParentCache(cache)
	, mPageSize(10000)
	, mAllPagesLoaded(false)
	, amInBatchMode(false)
	, needToFlush(false)
{
}

template< class T >
OmPagingStore<T>::~OmPagingStore()
{
	foreach( const PageNum & pageNum, loadedPageNumbers ){
		
		for( quint32 i = 0; i < mPageSize; ++i ){
			delete mValueToSegPtrHash[pageNum][i];
		}
	}
}

template< class T >
void OmPagingStore<T>::SaveAllLoadedPages()
{
	foreach( const PageNum & pageNum, loadedPageNumbers ){
		doSaveSegmentPage( pageNum );
	}
}

template< class T >
void OmPagingStore<T>::SaveDirtySegmentPages()
{
	foreach( const PageNum & pageNum, dirtySegmentPages ){
		doSaveSegmentPage( pageNum );
	}
	dirtySegmentPages.clear();
}

template< class T >
void OmPagingStore<T>::doSaveSegmentPage( const PageNum pageNum )
{
	const std::vector<T*> & page = mValueToSegPtrHash[ pageNum ];
	OmDataArchiveSegment::ArchiveWrite( OmDataPaths::getSegmentPagePath( mSegmentation->GetId(), pageNum ),
					    page, mParentCache );
}

template< class T >
void OmPagingStore<T>::LoadValuePage( const PageNum pageNum )
{
	std::vector<T*> & page = mValueToSegPtrHash[ pageNum ];
	page.resize( mPageSize, NULL );

	OmDataArchiveSegment::ArchiveRead( OmDataPaths::getSegmentPagePath( mSegmentation->GetId(), pageNum ),
					   page, mParentCache );
	
	loadedPageNumbers.insert( pageNum );

	if( loadedPageNumbers == validPageNumbers ){
		mAllPagesLoaded = true;
	}
}

template< class T >
void OmPagingStore<T>::AddItem( T* item )
{
	const OmSegID value = item->getValue();

	const PageNum pageNum = getValuePageNum(value);

	if( !validPageNumbers.contains( pageNum ) ) {
		mValueToSegPtrHash[ pageNum ].resize(mPageSize, NULL);
		validPageNumbers.insert( pageNum );
		loadedPageNumbers.insert( pageNum );
	}

	mValueToSegPtrHash[ pageNum ][value % mPageSize] = item;
}

template< class T >
bool OmPagingStore<T>::IsValueAlreadyMapped( const OmSegID value )
{
	if (0 == value) {
		return false;
	}
	
	const PageNum pageNum = getValuePageNum(value);
	
	if( !validPageNumbers.contains( pageNum ) ){
		return false;
	}
	
	if( !loadedPageNumbers.contains( pageNum ) ){
		LoadValuePage( pageNum );
	}
	
	if( NULL != mValueToSegPtrHash[pageNum][value % mPageSize]){
		return true;
	}
	
	return false;
}

template< class T >
void OmPagingStore<T>::AddToDirtyList( const OmSegID value )
{
	if( amInBatchMode ){
		needToFlush = true;
	} else {
		dirtySegmentPages.insert( getValuePageNum( value ) );
	}
}

template< class T >
void OmPagingStore<T>::FlushDirtyItems()
{
	if( amInBatchMode ){
		if( !needToFlush ){
			return;
		}
		SaveAllLoadedPages();
		needToFlush = false;
	} else {
		SaveDirtySegmentPages();
	}
}

template< class T >
void OmPagingStore<T>::SetBatchMode( const bool batchMode )
{
	amInBatchMode = batchMode;
}

template< class T >
T* OmPagingStore<T>::GetSegmentFromValue(const OmSegID value ) {
	if( !mAllPagesLoaded ){
		if ( !IsValueAlreadyMapped( value ) ){
			return NULL;
		}
	}
	return mValueToSegPtrHash[ getValuePageNum(value) ][ value % mPageSize];
}

template< class T >
PageNum OmPagingStore<T>::getValuePageNum( const OmSegID value ){
	return PageNum(value / mPageSize);
}
