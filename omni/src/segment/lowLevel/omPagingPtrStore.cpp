#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/omSegmentCache.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"

static const quint32 DEFAULT_PAGE_SIZE = 100000;
static const quint32 DEFAULT_VECTOR_SIZE = DEFAULT_PAGE_SIZE;

OmPagingPtrStore::OmPagingPtrStore(OmSegmentation * segmentation)
	: mSegmentation(segmentation)
	, mPageSize(DEFAULT_PAGE_SIZE)
	, mAllPagesLoaded(false)
	, amInBatchMode(false)
	, needToFlush(false)
{
	mValueToSeg.resize(DEFAULT_VECTOR_SIZE);
}

void OmPagingPtrStore::SaveAllLoadedPages()
{
	foreach( const PageNum & pageNum, loadedPageNumbers ){
		doSavePage(pageNum);
	}
}

void OmPagingPtrStore::SaveDirtyPages()
{
	foreach( const PageNum & pageNum, dirtyPages ){
		doSavePage(pageNum);
	}
	dirtyPages.clear();
}

void OmPagingPtrStore::doSavePage(const PageNum pageNum)
{
	mValueToSeg[pageNum].Save();
}

void OmPagingPtrStore::loadValuePage(const PageNum pageNum)
{
	loadValuePage(pageNum, om::DONT_REWRITE_SEGMENTS);
}

void OmPagingPtrStore::loadValuePage(const PageNum pageNum,
									 const om::RewriteSegments rewriteSegments)
{
	resizeVectorIfNeeded(pageNum);

	mValueToSeg[pageNum] = OmSegmentPage(mSegmentation,
										 pageNum,
										 mPageSize);
	mValueToSeg[pageNum].Load(rewriteSegments);

	loadedPageNumbers.insert(pageNum);

	if( loadedPageNumbers == validPageNumbers ){
		mAllPagesLoaded = true;
	}
}

OmSegment* OmPagingPtrStore::AddItem(const OmSegment& item)
{
	const OmSegID value = item.value();

	const PageNum pageNum = getValuePageNum(value);

	if( !validPageNumbers.contains(pageNum) ) {
		resizeVectorIfNeeded(pageNum);
		mValueToSeg[pageNum] = OmSegmentPage(mSegmentation,
											 pageNum,
											 mPageSize);
		mValueToSeg[pageNum].Create();
		validPageNumbers.insert(pageNum);
		loadedPageNumbers.insert(pageNum);
	}

	mValueToSeg[pageNum][value % mPageSize] = item;

	return &(mValueToSeg[pageNum][value % mPageSize]);
}

bool OmPagingPtrStore::IsValueAlreadyMapped( const OmSegID value )
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

void OmPagingPtrStore::AddToDirtyList(const OmSegID value)
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

void OmPagingPtrStore::FlushDirtyItems()
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

void OmPagingPtrStore::SetBatchMode(const bool batchMode)
{
	amInBatchMode = batchMode;
}

OmSegment* OmPagingPtrStore::GetItemFromValue(const OmSegID value)
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

void OmPagingPtrStore::resizeVectorIfNeeded(const PageNum pageNum)
{
	if( pageNum >= mValueToSeg.size() ){
		mValueToSeg.resize(pageNum*2);
	}
}

void OmPagingPtrStore::UpgradeSegmentSerialization()
{
	foreach(PageNum pn, validPageNumbers){
		loadValuePage(pn, om::REWRITE_SEGMENTS);
	}
}
