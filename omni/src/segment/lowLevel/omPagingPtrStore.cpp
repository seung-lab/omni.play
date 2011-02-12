#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/omSegmentCache.h"
#include "volume/omSegmentation.h"
#include "utility/omTimer.hpp"

static const uint32_t DEFAULT_PAGE_SIZE = 100000; // about 4.8 MB on disk
static const uint32_t DEFAULT_PAGE_VECTOR_SIZE = 20;

OmPagingPtrStore::OmPagingPtrStore(OmSegmentation * segmentation)
	: segmentation_(segmentation)
	, pageSize_(DEFAULT_PAGE_SIZE)
{
	pages_.resize(DEFAULT_PAGE_VECTOR_SIZE);
}

void OmPagingPtrStore::loadAllSegmentPages()
{
	OmTimer timer;

	loadMetadata();

	foreach(PageNum pageNum, validPageNumbers_){
		resizeVectorIfNeeded(pageNum);

		pages_[pageNum] = OmSegmentPage(segmentation_,
										pageNum,
										pageSize_);
		pages_[pageNum].Load();
	}

	printf("loaded %d segment pages (%.6f secs) \n",
		   validPageNumbers_.size(), timer.s_elapsed());
}

OmSegment* OmPagingPtrStore::AddSegment(const OmSegID value)
{
	const PageNum pageNum = getValuePageNum(value);

	if( !validPageNumbers_.contains(pageNum) ) {
		resizeVectorIfNeeded(pageNum);
		validPageNumbers_.insert(pageNum);

		pages_[pageNum] = OmSegmentPage(segmentation_,
										pageNum,
										pageSize_);
		pages_[pageNum].Create();

		storeMetadata();
	}

	OmSegment* ret = &(pages_[pageNum][ value % pageSize_]);
	ret->data_->value = value;

	return ret;
}

/**
 * returns NULL if segment was never instantiated;
 **/
OmSegment* OmPagingPtrStore::GetSegment(const OmSegID value)
{
	if(!value){
		return NULL;
	}

	const PageNum pageNum = getValuePageNum(value);
	if(!validPageNumbers_.contains(pageNum)){
		return NULL;
	}

	OmSegment* ret = &(pages_[pageNum][ value % pageSize_]);
	if(!ret->data_->value){
		return NULL;
	}

	return ret;
}

void OmPagingPtrStore::resizeVectorIfNeeded(const PageNum pageNum)
{
	if( pageNum >= pages_.size() ){
		pages_.resize(pageNum*2);
	}
}

QString OmPagingPtrStore::metadataPathQStr(){
	const QString volPath = OmFileNames::MakeVolSegmentsPath(segmentation_);
	return QString("%1/segment_pages.data").arg(volPath);
}

void OmPagingPtrStore::loadMetadata()
{
	QFile file(metadataPathQStr());

	if(!file.open(QIODevice::ReadOnly)){
		throw OmIoException("error reading file", metadataPathQStr());
	}

	QDataStream in(&file);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);

	int version;

	in >> version;
	in >> pageSize_;
	in >> validPageNumbers_;

	if(!in.atEnd()){
		throw OmIoException("corrupt file?", metadataPathQStr());
	}
}

void OmPagingPtrStore::storeMetadata()
{
	QFile file(metadataPathQStr());

	if (!file.open(QIODevice::WriteOnly)) {
		throw OmIoException("could not write file", metadataPathQStr());
	}

	QDataStream out(&file);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);

	static const int version = 1;

	out << version;
	out << pageSize_;
	out << validPageNumbers_;
}

void OmPagingPtrStore::Flush()
{
	foreach(PageNum pageNum, validPageNumbers_){
		pages_[pageNum].Flush();
	}
}
