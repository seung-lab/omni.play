#pragma once

#include "common/common.h"

namespace om {
namespace segmentation { class loader; }
namespace volume { class segmentation; }

namespace segment {

class page;

class pagingPtrStore {
public:
    pagingPtrStore(volume::segmentation*);

    ~pagingPtrStore();

    void Flush();

    uint32_t PageSize(){
        return pageSize_;
    }

    segment AddSegment(const common::segId value);

    std::vector<page*> Pages(){
        if(!loaded_) {
            loadAllSegmentPages();
            loaded_ = true;
        }

        return pages_;
    }

    uint32_t NumPages(){
        return pages_.size();
    }

    const std::set<common::pageNum> ValidPageNums() const {
        return validPageNums_;
    }

    volume::segmentation* Vol() const {
        return vol_;
    }

private:
    volume::segmentation *const vol_;

    uint32_t pageSize_;
    std::vector<page*> pages_;
    bool loaded_;

    std::set<common::pageNum> validPageNums_;

    void loadAllSegmentPages();
    void resizeVectorIfNeeded(const common::pageNum pageNum);

    std::string metadataPathQStr();
    void loadMetadata();
    void storeMetadata();

    void loadPage(const common::pageNum page);

    friend class om::segmentation::loader;
};

} // namespace segment
} // namespace om
