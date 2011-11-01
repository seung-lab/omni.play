#include "segment/segment.h"
#include "segment/io/segmentPage.hpp"
#include "segment/lowLevel/pagingPtrStore.h"
#include "volume/segmentation.h"
#include "threads/taskManagerTypes.h"
#include "threads/taskManager.hpp"
#include "common/string.hpp"

#include "boost/filesystem.hpp"
#include "boost/regex.hpp"

namespace om {
namespace segment {

static const uint32_t DEFAULT_PAGE_SIZE = 100000; // about 4.8 MB on disk

pagingPtrStore::pagingPtrStore(volume::segmentation* vol)
    : vol_(vol)
    , pageSize_(DEFAULT_PAGE_SIZE)
    , loaded_(false)
{}

pagingPtrStore::~pagingPtrStore()
{
    FOR_EACH(iter, validPageNums_){
        delete pages_[*iter];
    }
}

void pagingPtrStore::loadAllSegmentPages()
{
    loadMetadata();

    const common::pageNum maxNum = *std::max_element(validPageNums_.begin(),
                                                     validPageNums_.end());
    resizeVectorIfNeeded(maxNum);

    threads::threadPool pool;
    pool.start();

    FOR_EACH(iter, validPageNums_)
    {
        const common::pageNum pageNum = *iter;

        pool.push_back(
            zi::run_fn(
                zi::bind(&pagingPtrStore::loadPage,
                         this, pageNum)));
    }

    pool.join();
}

void pagingPtrStore::loadPage(const common::pageNum pageNum)
{
    pages_[pageNum] = new page(vol_,
                               pageNum,
                               pageSize_);
    pages_[pageNum]->Load();
}

segment pagingPtrStore::AddSegment(const common::segId value)
{
    const common::pageNum pageNum = value / pageSize_;

    if(!validPageNums_.count(pageNum))
    {
        resizeVectorIfNeeded(pageNum);
        validPageNums_.insert(pageNum);

        pages_[pageNum] = new page(vol_,
                                   pageNum,
                                   pageSize_);
        pages_[pageNum]->Create();

        storeMetadata();
    }

    page& page = *pages_[pageNum];
    segment ret = page[ value % pageSize_];

    //ret.data_->value = value;

    return ret;
}

void pagingPtrStore::resizeVectorIfNeeded(const common::pageNum pageNum)
{
    if( pageNum >= pages_.size() ){
        pages_.resize( (1+pageNum) * 2 );
    }
}

std::string pagingPtrStore::metadataPathQStr()
{
    return vol_->Folder()->GetVolSegmentsPathAbs("segment_pages.data");
}

void pagingPtrStore::loadMetadata()
{
    using namespace boost::filesystem;
    pageSize_ = 100000; // default page size.  TODO: read from disk.
    path p = vol_->Folder()->GetVolSegmentsPathAbs();

    boost::regex e("segment_page([0-9]+).data.ver4");

    if (exists(p))
    {
        for(directory_iterator it = directory_iterator(p); it != directory_iterator(); it++)
        {
            boost::smatch what;
            if( boost::regex_search(it->path().string(), what, e))
            {
                uint32_t num = string::toNum<uint32_t>(what[1].str());
                validPageNums_.insert(num);
            }
        }
    }
}

void pagingPtrStore::storeMetadata()
{
}

void pagingPtrStore::Flush()
{
    if(validPageNums_.empty()){
        storeMetadata();

    } else {
        FOR_EACH(iter, validPageNums_){
            pages_[*iter]->Flush();
        }
    }
}

} // namespace segment
} // namespace om
