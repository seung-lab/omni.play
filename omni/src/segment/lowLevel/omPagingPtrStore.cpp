#include "segment/lowLevel/omPagingPtrStore.h"
#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataPath.h"
#include "segment/omSegmentCache.h"
#include "volume/omSegmentation.h"

static const quint32 DEFAULT_PAGE_SIZE = 100000;
static const quint32 DEFAULT_VECTOR_SIZE = DEFAULT_PAGE_SIZE;

template< class T >
OmPagingPtrStore<T>::OmPagingPtrStore(OmSegmentation * segmentation, OmSegmentCache * cache)
	: mSegmentation(segmentation)
	, mParentCache(cache)
	, mPageSize(DEFAULT_PAGE_SIZE)
	, mAllPagesLoaded(false)
	, amInBatchMode(false)
	, needToFlush(false)
{
	mValueToSegPtr.resize(DEFAULT_VECTOR_SIZE);
}

template< class T >
OmPagingPtrStore<T>::~OmPagingPtrStore()
{
	foreach( const PageNum & pageNum, loadedPageNumbers ){
		for( quint32 i = 0; i < mPageSize; ++i ){
			delete mValueToSegPtr[pageNum][i];
		}
	}
}

template< class T >
void OmPagingPtrStore<T>::SaveAllLoadedPages()
{
	foreach( const PageNum & pageNum, loadedPageNumbers ){
		doSavePage(pageNum);
	}
}

template< class T >
void OmPagingPtrStore<T>::SaveDirtyPages()
{
	foreach( const PageNum & pageNum, dirtyPages ){
		doSavePage(pageNum);
	}
	dirtyPages.clear();
}

template< class T >
void OmPagingPtrStore<T>::doSavePage( const PageNum pageNum )
{
	const std::vector<T*> & page = mValueToSegPtr[pageNum];
	OmDataArchiveSegment::ArchiveWrite( OmDataPaths::getSegmentPagePath( mSegmentation->GetId(), pageNum ),
					    page, mParentCache );
}

template< class T >
void OmPagingPtrStore<T>::LoadValuePage( const PageNum pageNum )
{
	resizeVectorIfNeeded(pageNum);

	std::vector<T*> & page = mValueToSegPtr[pageNum];
	page.resize( mPageSize, NULL );

	OmDataArchiveSegment::ArchiveRead( OmDataPaths::getSegmentPagePath( mSegmentation->GetId(),pageNum),
					   page, mParentCache );

	loadedPageNumbers.insert(pageNum);

	if( loadedPageNumbers == validPageNumbers ){
		mAllPagesLoaded = true;
	}
}

template< class T >
void OmPagingPtrStore<T>::AddItem( T* item )
{
	const OmSegID value = item->getValue();

	const PageNum pageNum = getValuePageNum(value);

	if( !validPageNumbers.contains(pageNum) ) {
		resizeVectorIfNeeded(pageNum);
		mValueToSegPtr[pageNum].resize(mPageSize, NULL);
		validPageNumbers.insert(pageNum);
		loadedPageNumbers.insert(pageNum);
	}

	mValueToSegPtr[pageNum][value % mPageSize] = item;
}

template< class T >
bool OmPagingPtrStore<T>::IsValueAlreadyMapped( const OmSegID value )
{
	if (0 == value) {
		return false;
	}

	const PageNum pageNum = getValuePageNum(value);

	if( !validPageNumbers.contains(pageNum) ){
		return false;
	}

	if( !loadedPageNumbers.contains(pageNum) ){
		LoadValuePage(pageNum);
	}

	if( NULL != mValueToSegPtr[pageNum][value % mPageSize]){
		return true;
	}

	return false;
}

template< class T >
void OmPagingPtrStore<T>::AddToDirtyList(const OmSegID value)
{
	if( amInBatchMode ){
		needToFlush = true;
	} else {
		dirtyPages.insert( getValuePageNum( value ) );
	}
}

template< class T >
void OmPagingPtrStore<T>::FlushDirtyItems()
{
	if( amInBatchMode ){
		if( !needToFlush ){
			return;
		}
		SaveAllLoadedPages();
		needToFlush = false;
	} else {
		SaveDirtyPages();
	}
}

template< class T >
void OmPagingPtrStore<T>::SetBatchMode(const bool batchMode)
{
	amInBatchMode = batchMode;
}

template< class T >
T* OmPagingPtrStore<T>::GetItemFromValue(const OmSegID value)
{
	if( !mAllPagesLoaded ){
		if ( !IsValueAlreadyMapped( value ) ){
			return NULL;
		}
	}
	return mValueToSegPtr[ getValuePageNum(value) ][ value % mPageSize];
}

template< class T >
PageNum OmPagingPtrStore<T>::getValuePageNum(const OmSegID value)
{
	PageNum pn = PageNum(value / mPageSize);
	if(!validPageNumbers.contains(pn)){
		throw OmIoException("bad page number");
	}
	return pn;
}

template< class T >
void OmPagingPtrStore<T>::resizeVectorIfNeeded(const PageNum pageNum)
{
	if( pageNum >= mValueToSegPtr.size() ){
		mValueToSegPtr.resize(pageNum*2);
	}
}
