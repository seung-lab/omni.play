#include "segment/lowLevel/omPagingPtrStore.h"
#include "utility/omSimpleProgress.hpp"
#include "volume/omSegmentation.h"

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
    loadMetadata();

    const PageNum maxNum = *std::max_element(validPageNums_.begin(),
                                             validPageNums_.end());
    resizeVectorIfNeeded(maxNum);
    std::cout << "max pageNum: " << maxNum << "\n";

    OmSimpleProgress prog(validPageNums_.size(), "Segment page load");

    OmThreadPool pool;
    pool.start();

    FOR_EACH(iter, validPageNums_)
    {
        const PageNum pageNum = *iter;

        pool.push_back(
            zi::run_fn(
                zi::bind(&OmPagingPtrStore::loadPage,
                         this, pageNum, &prog)));
    }

    pool.join();
    prog.Join();
}

void OmPagingPtrStore::loadPage(const PageNum pageNum, OmSimpleProgress* prog)
{
    pages_[pageNum] = OmSegmentPage(segmentation_,
                                    pageNum,
                                    pageSize_);
    pages_[pageNum].Load();

    prog->DidOne();
}

OmSegment* OmPagingPtrStore::AddSegment(const OmSegID value)
{
    const PageNum pageNum = getValuePageNum(value);

    if(!validPageNums_.count(pageNum))
    {
        resizeVectorIfNeeded(pageNum);
        validPageNums_.insert(pageNum);

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

void OmPagingPtrStore::resizeVectorIfNeeded(const PageNum pageNum)
{
    if( pageNum >= pages_.size() ){
        pages_.resize(pageNum*2);
    }
}

QString OmPagingPtrStore::metadataPathQStr()
{
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

    QSet<PageNum> validPageNumbers;
    in >> validPageNumbers;
    FOR_EACH(iter, validPageNumbers){
        validPageNums_.insert(*iter);
    }

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

    QSet<PageNum> validPageNumbers;
    FOR_EACH(iter, validPageNums_){
        validPageNumbers.insert(*iter);
    }
    out << validPageNumbers;
}

void OmPagingPtrStore::Flush()
{
    FOR_EACH(iter, validPageNums_){
        pages_[*iter].Flush();
    }
}
