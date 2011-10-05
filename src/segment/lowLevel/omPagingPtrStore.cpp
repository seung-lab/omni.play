#include "segment/omSegment.h"
#include "segment/io/omSegmentPage.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "utility/omSimpleProgress.hpp"
#include "volume/segmentation.h"

#include <QSet>

static const uint32_t DEFAULT_PAGE_SIZE = 100000; // about 4.8 MB on disk

OmPagingPtrStore::OmPagingPtrStore(segmentation* vol)
    : vol_(vol)
    , pageSize_(DEFAULT_PAGE_SIZE)
{}

OmPagingPtrStore::~OmPagingPtrStore()
{
    FOR_EACH(iter, validPageNums_){
        delete pages_[*iter];
    }
}

void OmPagingPtrStore::loadAllSegmentPages()
{
    loadMetadata();

    const PageNum maxNum = *std::max_element(validPageNums_.begin(),
                                             validPageNums_.end());
    resizeVectorIfNeeded(maxNum);

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
    pages_[pageNum] = new OmSegmentPage(vol_,
                                        pageNum,
                                        pageSize_);
    pages_[pageNum]->Load();

    prog->DidOne();
}

OmSegment* OmPagingPtrStore::AddSegment(const segId value)
{
    const PageNum pageNum = value / pageSize_;

    if(!validPageNums_.count(pageNum))
    {
        resizeVectorIfNeeded(pageNum);
        validPageNums_.insert(pageNum);

        pages_[pageNum] = new OmSegmentPage(vol_,
                                            pageNum,
                                            pageSize_);
        pages_[pageNum]->Create();

        storeMetadata();
    }

    OmSegmentPage& page = *pages_[pageNum];
    OmSegment* ret = &(page[ value % pageSize_]);

    ret->data_->value = value;

    return ret;
}

void OmPagingPtrStore::resizeVectorIfNeeded(const PageNum pageNum)
{
    if( pageNum >= pages_.size() ){
        pages_.resize( (1+pageNum) * 2 );
    }
}

QString OmPagingPtrStore::metadataPathQStr()
{
    return QString::fromStdString(
        vol_->Folder()->GetVolSegmentsPathAbs("segment_pages.data")
        );
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
    const QString path = metadataPathQStr();

    QFile file(path);

    om::file::openFileWO(file);

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
    if(validPageNums_.empty()){
        storeMetadata();

    } else {
        FOR_EACH(iter, validPageNums_){
            pages_[*iter]->Flush();
        }
    }
}
