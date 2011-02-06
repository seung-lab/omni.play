#ifndef OM_PAGING_PTR_STORE_H
#define OM_PAGING_PTR_STORE_H

#include "common/om.hpp"
#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "segment/io/omSegmentPage.hpp"

#include <QSet>

class OmDataPath;
class OmSegmentation;
class OmSegmentCache;

class OmPagingPtrStore {
public:
    OmPagingPtrStore(OmSegmentation*);
    virtual ~OmPagingPtrStore(){}

    void Flush();

    quint32 getPageSize() {
        return pageSize_;
    }

    OmSegment* AddSegment(const OmSegID value);

    /**
     * returns NULL if segment was never instantiated;
     **/
    inline OmSegment* GetSegment(const OmSegID value)
    {
        if(!value){
            return NULL;
        }

        const PageNum pageNum = value / pageSize_;
        if(!validPageNumbers_.contains(pageNum)){
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

private:
    OmSegmentation *const segmentation_;

    uint32_t pageSize_;
    std::vector<OmSegmentPage> pages_;
    QSet<PageNum> validPageNumbers_;

    inline PageNum getValuePageNum(const OmSegID value){
        return PageNum(value / pageSize_);
    }

    void loadAllSegmentPages();
    void resizeVectorIfNeeded(const PageNum pageNum);

    QString metadataPathQStr();
    void loadMetadata();
    void storeMetadata();

    friend QDataStream &operator<< (QDataStream& out, const OmPagingPtrStore&);
    friend QDataStream &operator>> (QDataStream& in, OmPagingPtrStore&);
};

#endif
