#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/omSegmentCache.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"

static const quint32 DEFAULT_PAGE_SIZE = 100000;
static const quint32 DEFAULT_VECTOR_SIZE = DEFAULT_PAGE_SIZE;

template<typename T>
OmPagingPtrStore<T>::OmPagingPtrStore(OmSegmentation * segmentation)
	: mSegmentation(segmentation)
	, mPageSize(DEFAULT_PAGE_SIZE)
	, mAllPagesLoaded(false)
	, amInBatchMode(false)
	, needToFlush(false)
{
	mValueToSeg.resize(DEFAULT_VECTOR_SIZE);
}

template<typename T>
void OmPagingPtrStore<T>::SaveAllLoadedPages()
{
	foreach( const PageNum & pageNum, loadedPageNumbers ){
		doSavePage(pageNum);
	}
}

template<typename T>
void OmPagingPtrStore<T>::SaveDirtyPages()
{
	foreach( const PageNum & pageNum, dirtyPages ){
		doSavePage(pageNum);
	}
	dirtyPages.clear();
}

template<typename T>
OmDataPath OmPagingPtrStore<T>::getPath(const PageNum pageNum)
{
	return OmDataPaths::getSegmentPagePath( mSegmentation->GetID(), pageNum);
}

template<typename T>
void OmPagingPtrStore<T>::doSavePage( const PageNum pageNum )
{
	const OmSegmentPage& page = mValueToSeg[pageNum];
	OmDataArchiveSegment::ArchiveWrite(getPath(pageNum),
									   page,
									   mSegmentation->GetSegmentCache());
}

template<typename T>
void OmPagingPtrStore<T>::loadValuePage(const PageNum pageNum)
{
	loadValuePage(pageNum, om::DONT_REWRITE_SEGMENTS);
}

template<typename T>
void OmPagingPtrStore<T>::loadValuePage(const PageNum pageNum,
										const om::RewriteSegments rewriteSegments)
{
	resizeVectorIfNeeded(pageNum);

	mValueToSeg[pageNum] = OmSegmentPage(mPageSize);
	OmSegmentPage& page = mValueToSeg[pageNum];

	OmDataArchiveSegment::ArchiveRead(getPath(pageNum),
									  page,
									  mSegmentation->GetSegmentCache(),
									  rewriteSegments);

	loadedPageNumbers.insert(pageNum);

	if( loadedPageNumbers == validPageNumbers ){
		mAllPagesLoaded = true;
	}
}

template<typename T>
T* OmPagingPtrStore<T>::AddItem(const T& item)
{
	const OmSegID value = item.value();

	const PageNum pageNum = getValuePageNum(value);

	if( !validPageNumbers.contains(pageNum) ) {
		resizeVectorIfNeeded(pageNum);
		mValueToSeg[pageNum] = OmSegmentPage(mPageSize);
		validPageNumbers.insert(pageNum);
		loadedPageNumbers.insert(pageNum);
	}

	mValueToSeg[pageNum][value % mPageSize] = item;

	return &(mValueToSeg[pageNum][value % mPageSize]);
}

template<typename T>
bool OmPagingPtrStore<T>::IsValueAlreadyMapped( const OmSegID value )
{
	if (0 == value) {
		return false;
	}

	const PageNum pageNum = getValuePageNum(value);
	if(!validPageNumbers.contains(pageNum)){
		return false;
	}

	if(!loadedPageNumbers.contains(pageNum)){
		loadValuePage(pageNum);
	}

	if( 0 != mValueToSeg[pageNum][value % mPageSize].value()){
		return true;
	}

	return false;
}

template<typename T>
void OmPagingPtrStore<T>::AddToDirtyList(const OmSegID value)
{
	if( amInBatchMode ){
		needToFlush = true;
	} else {
		PageNum pn = getValuePageNum(value);
		if(!validPageNumbers.contains(pn)){
			throw OmIoException("bad page number " +
								boost::lexical_cast<std::string>(pn)
								+ " for value " +
								boost::lexical_cast<std::string>(value));
		}
		dirtyPages.insert(pn);
	}
}

template<typename T>
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

template<typename T>
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

	const PageNum pn = getValuePageNum(value);
	if(!validPageNumbers.contains(pn)){
		throw OmIoException("bad page number " +
							boost::lexical_cast<std::string>(pn)
							+ " for value " +
							boost::lexical_cast<std::string>(value));
	}

	return &(mValueToSeg[pn][ value % mPageSize]);
}

template<typename T>
void OmPagingPtrStore<T>::resizeVectorIfNeeded(const PageNum pageNum)
{
	if( pageNum >= mValueToSeg.size() ){
		mValueToSeg.resize(pageNum*2);
	}
}

template<typename T>
void OmPagingPtrStore<T>::UpgradeSegmentSerialization()
{
	foreach(PageNum pn, validPageNumbers){
		loadValuePage(pn, om::REWRITE_SEGMENTS);
	}
}
