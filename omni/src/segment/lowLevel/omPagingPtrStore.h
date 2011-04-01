#ifndef OM_PAGING_PTR_STORE_H
#define OM_PAGING_PTR_STORE_H

#include "common/om.hpp"
#include "common/omCommon.h"

class OmSegmentPage;
class OmSimpleProgress;

class OmPagingPtrStore {
public:
    OmPagingPtrStore(OmSegmentation*);

    ~OmPagingPtrStore();

    void Flush();

    uint32_t PageSize(){
        return pageSize_;
    }

    OmSegment* AddSegment(const OmSegID value);

    std::vector<OmSegmentPage*> Pages(){
        return pages_;
    }

    uint32_t NumPages(){
        return pages_.size();
    }

    const std::set<PageNum> ValidPageNums() const {
        return validPageNums_;
    }

private:
    OmSegmentation *const segmentation_;

    uint32_t pageSize_;
    std::vector<OmSegmentPage*> pages_;

    std::set<PageNum> validPageNums_;

    void loadAllSegmentPages();
    void resizeVectorIfNeeded(const PageNum pageNum);

    QString metadataPathQStr();
    void loadMetadata();
    void storeMetadata();

    void loadPage(const PageNum page, OmSimpleProgress* prog);

    friend QDataStream &operator<< (QDataStream& out, const OmPagingPtrStore&);
    friend QDataStream &operator>> (QDataStream& in, OmPagingPtrStore&);
};

#endif
