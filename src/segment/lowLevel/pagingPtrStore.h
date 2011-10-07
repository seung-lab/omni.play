#pragma once

#include "common/om.hpp"
#include "common/common.h"

class segmentPage;
class OmSimpleProgress;
class segmentation;

namespace om { namespace segmentation { class loader; } }

class pagingPtrStore {
public:
    pagingPtrStore(segmentation*);

    ~pagingPtrStore();

    void Flush();

    uint32_t PageSize(){
        return pageSize_;
    }

    segment* AddSegment(const segId value);

    std::vector<segmentPage*> Pages(){
        return pages_;
    }

    uint32_t NumPages(){
        return pages_.size();
    }

    const std::set<PageNum> ValidPageNums() const {
        return validPageNums_;
    }

    segmentation* Vol() const {
        return vol_;
    }

private:
    segmentation *const vol_;

    uint32_t pageSize_;
    std::vector<segmentPage*> pages_;

    std::set<PageNum> validPageNums_;

    void loadAllSegmentPages();
    void resizeVectorIfNeeded(const PageNum pageNum);

    std::string metadataPathQStr();
    void loadMetadata();
    void storeMetadata();

    void loadPage(const PageNum page, OmSimpleProgress* prog);

    friend class om::segmentation::loader;
};

