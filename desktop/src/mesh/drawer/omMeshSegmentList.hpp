#pragma once

#include "chunks/omSegChunk.h"
#include "common/common.h"
#include "common/omDebug.h"
#include "mesh/drawer/omMeshSegmentListTask.h"
#include "mesh/drawer/omMeshSegmentListTypes.hpp"
#include "threads/omTaskManager.hpp"
#include "volume/omMipVolume.h"
#include "zi/omMutex.h"

class OmMeshSegmentList {
private:
    static const int MAX_THREADS = 3;
    OmSegmentation *const segmentation_;

public:
    OmMeshSegmentList(OmSegmentation* segmentation)
        : segmentation_(segmentation)
    {
        threadPool_.start(MAX_THREADS);
    }

    ~OmMeshSegmentList()
    {
        threadPool_.clear();
        threadPool_.stop();
    }

    boost::optional<OmSegPtrList>
    GetFromCacheIfReady(OmSegChunk* chunk, OmSegment* rootSeg)
    {
        zi::guard g(lock_);

        OmSegPtrListValid& spList =
            mSegmentListCache[makeKey(chunk, rootSeg)];

        if(spList.isFetching){ // coord already in queue to be fetched
            return boost::optional<OmSegPtrList>();
        }

        // remove from cache if freshness is too old
        const uint32_t currentFreshness = rootSeg->getFreshnessForMeshes();
        if(spList.isValid && currentFreshness != spList.freshness)
        {
            spList.list.clear();
            spList.isValid = false;
        }

        if(!spList.isValid)
        { // add coord to list to be fetched
            spList = OmSegPtrListValid(true);

            om::shared_ptr<OmMeshSegmentListTask> task
                = om::make_shared<OmMeshSegmentListTask>(chunk,
                                                         rootSeg,
                                                         this,
                                                         segmentation_);

            threadPool_.push_back(task);
            return boost::optional<OmSegPtrList>();
        }

        // coord was valid
        return boost::optional<OmSegPtrList>(spList.list);
    }

    void AddToCache(OmSegChunk* chunk, OmSegment* rootSeg,
                    const OmSegPtrList & segmentsToDraw)
    {
        zi::guard g(lock_);

        mSegmentListCache[makeKey(chunk, rootSeg)]
            = OmSegPtrListValid(segmentsToDraw,
                                rootSeg->getFreshnessForMeshes());
    }

private:
    std::map<OmMeshSegListKey, OmSegPtrListValid> mSegmentListCache;
    OmThreadPool threadPool_;
    zi::mutex lock_;

    OmMeshSegListKey makeKey(OmSegChunk* chunk, OmSegment* rootSeg)
    {
        const om::coords::Chunk& c = chunk->GetCoordinate();
        return OmMeshSegListKey(rootSeg->GetSegmentationID(),
                                rootSeg->value(),
                                c.Level,
                                c.Coordinate.x,
                                c.Coordinate.y,
                                c.Coordinate.z);
    }
};

