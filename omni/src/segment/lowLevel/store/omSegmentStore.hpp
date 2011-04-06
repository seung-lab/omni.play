#ifndef OM_SEGMENT_STORE_HPP
#define OM_SEGMENT_STORE_HPP

#include "common/omCommon.h"
#include "zi/omMutex.h"

class OmCacheSegRootIDs;
class OmCacheSegStore;
class OmPagingPtrStore;
class OmSegment;
class OmSegmentPage;
class OmSegmentation;
class OmSegmentsImpl;

class OmSegmentsStore {
private:
    OmSegmentation *const segmentation_;

    const boost::scoped_ptr<OmPagingPtrStore> segmentPagesPtr_;
    OmPagingPtrStore *const segmentPages_;
    zi::spinlock pagesLock_;

    boost::scoped_ptr<OmCacheSegStore> cachedStore_;
    boost::scoped_ptr<OmCacheSegRootIDs> cacheRootIDs_;


public:
    OmSegmentsStore(OmSegmentation* segmentation);
    ~OmSegmentsStore();

// pages
    uint32_t PageSize();
    uint32_t NumPages();
    std::vector<OmSegmentPage*> Pages();
    void Flush();

// segments
    OmSegment* AddSegment(const OmSegID value);

// caching
    void StartCaches();
    OmSegment* GetSegment(const OmSegID value);
    OmSegment* GetSegmentUnsafe(const OmSegID value);

    // WARNING: do not call from inside OmSegmentsImpl or OmSegmentsImplLowLevel
    OmSegID Root(const OmSegID segID);

private:
    friend QDataStream &operator<< (QDataStream& out, const OmSegmentsImpl&);
    friend QDataStream &operator>> (QDataStream& in, OmSegmentsImpl&);
};

#endif
