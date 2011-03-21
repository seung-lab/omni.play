#ifndef OM_PAGING_PTR_STORE_H
#define OM_PAGING_PTR_STORE_H

#include "common/om.hpp"
#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "segment/io/omSegmentPage.hpp"

#include <QSet>

class OmDataPath;
class OmSegmentation;
class OmSegments;
class OmSimpleProgress;

class OmPagingPtrStore {
public:
    OmPagingPtrStore(OmSegmentation*);
    virtual ~OmPagingPtrStore(){}

    void Flush();

    quint32 getPageSize() {
        return pageSize_;
    }

    OmSegment* AddSegment(const OmSegID value);

    std::vector<OmSegmentPage>& Pages(){
        return pages_;
    }

    const std::set<PageNum> ValidPageNums() const {
        return validPageNums_;
    }

    /**
     * returns NULL if segment was never instantiated;
     **/
    inline OmSegment* GetSegment(const OmSegID value)
    {
        if(!value){
            return NULL;
        }

        const PageNum pageNum = value / pageSize_;
        if(!validPageNums_.count(pageNum)){
            return NULL;
        }

        OmSegment* ret = &(pages_[pageNum][ value % pageSize_]);

        const OmSegID loadedID = ret->data_->value;
        if(!loadedID){
            return NULL;
        }
        if(loadedID != value){
            throw OmIoException("corruption detected in segment page");
        }

        return ret;
    }

    inline OmSegment* GetSegmentRaw(const OmSegID value) {
        return &(pages_[value / pageSize_][ value % pageSize_]);
    }

private:
    OmSegmentation *const segmentation_;

    uint32_t pageSize_;
    std::vector<OmSegmentPage> pages_;

    std::set<PageNum> validPageNums_;

    inline PageNum getValuePageNum(const OmSegID value){
        return PageNum(value / pageSize_);
    }

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
