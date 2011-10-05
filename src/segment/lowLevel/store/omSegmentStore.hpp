#pragma once

#include "common/common.h"
#include "zi/omMutex.h"
#include "datalayer/archive/segmentation.h"

class OmCacheSegRootIDs;
class OmCacheSegStore;
class OmPagingPtrStore;
class OmSegment;
class OmSegmentPage;
class segmentation;
class segmentsImpl;

class segmentsStore {
private:
    segmentation *const segmentation_;

    const boost::scoped_ptr<OmPagingPtrStore> segmentPagesPtr_;
    OmPagingPtrStore *const segmentPages_;
    zi::spinlock pagesLock_;

    boost::scoped_ptr<OmCacheSegStore> cachedStore_;
    boost::scoped_ptr<OmCacheSegRootIDs> cacheRootIDs_;

public:
    segmentsStore(segmentation* segmentation);
    ~segmentsStore();

// pages
    uint32_t PageSize();
    uint32_t NumPages();
    std::vector<OmSegmentPage*> Pages();
    void Flush();

// segments
    OmSegment* AddSegment(const segId value);
    bool IsSegmentValid(const segId value);

// caching
    void StartCaches();
    OmSegment* GetSegment(const segId value);
    OmSegment* GetSegmentUnsafe(const segId value);

    // WARNING: do not call from inside segmentsImpl or segmentsImplLowLevel
    segId Root(const segId segID);

private:
    friend YAML::Emitter &YAML::operator<<(YAML::Emitter& out, const segmentsImpl&);
    friend void YAML::operator>>(const YAML::Node& in, segmentsImpl&);
    friend QDataStream &operator<<(QDataStream& out, const segmentsImpl&);
    friend QDataStream &operator>>(QDataStream& in, segmentsImpl&);
};

