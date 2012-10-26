#pragma once

#include "common/common.h"
#include "zi/omMutex.h"
#include "datalayer/archive/segmentation.h"

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
    bool IsSegmentValid(const OmSegID value);

// caching
    void StartCaches();
    OmSegment* GetSegment(const OmSegID value);
    OmSegment* GetSegmentUnsafe(const OmSegID value);

    // WARNING: do not call from inside OmSegmentsImpl or OmSegmentsImplLowLevel
    OmSegID Root(const OmSegID segID);

private:
    friend YAML::Emitter &YAML::operator<<(YAML::Emitter& out, const OmSegmentsImpl&);
    friend void YAML::operator>>(const YAML::Node& in, OmSegmentsImpl&);
    friend QDataStream &operator<<(QDataStream& out, const OmSegmentsImpl&);
    friend QDataStream &operator>>(QDataStream& in, OmSegmentsImpl&);
};

