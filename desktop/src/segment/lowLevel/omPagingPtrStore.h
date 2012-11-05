#pragma once

#include "common/common.h"

class OmSegmentPage;
class OmSimpleProgress;
class OmSegmentation;

namespace om { namespace segmentation { class loader; } }

class OmPagingPtrStore {
public:
    OmPagingPtrStore(OmSegmentation*);

    ~OmPagingPtrStore();

    void Flush();

    uint32_t PageSize(){
        return pageSize_;
    }

    OmSegment* AddSegment(const om::common::SegID value);

    std::vector<OmSegmentPage*> Pages(){
        return pages_;
    }

    uint32_t NumPages(){
        return pages_.size();
    }

    const std::set<PageNum> ValidPageNums() const {
        return validPageNums_;
    }

    OmSegmentation* Vol() const {
        return vol_;
    }

private:
    OmSegmentation *const vol_;

    uint32_t pageSize_;
    std::vector<OmSegmentPage*> pages_;

    std::set<PageNum> validPageNums_;

    void loadAllSegmentPages();
    void resizeVectorIfNeeded(const PageNum pageNum);

    QString metadataPathQStr();
    void loadMetadata();
    void storeMetadata();

    void loadPage(const PageNum page, OmSimpleProgress* prog);

    friend class om::segmentation::loader;
};

