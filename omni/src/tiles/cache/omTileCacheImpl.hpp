#ifndef OM_TILE_CACHE_IMPL_HPP
#define OM_TILE_CACHE_IMPL_HPP

#include "threads/omTaskManager.hpp"
#include "tiles/cache/omTaskManagerContainerMipSorted.hpp"
#include "common/om.hpp"
#include "common/omCommon.h"
#include "system/omStateManager.h"
#include "tiles/cache/omTileCacheChannel.hpp"
#include "tiles/cache/omTileCacheEventListener.hpp"
#include "tiles/cache/omTileCacheSegmentation.hpp"
#include "tiles/omTilePreFetcher.h"
#include "utility/omLockedPODs.hpp"
#include "view2d/omTileDrawer.hpp"
#include "view2d/omView2dState.hpp"

#include <QApplication>

DECLARE_ZiARG_bool(noTilePrefetch);

/**
 * assumes called in a serialized fashion (as main QT GUI thread ensures)
 *
 * prefetcher calls in threaded-manner....
 **/

class OmTileCacheImpl {
private:
    boost::scoped_ptr<OmTilePreFetcher> preFetcher_;
    boost::scoped_ptr<OmTileCacheEventListener> listener_;

    OmTaskManager<OmTaskManagerContainerMipSorted> threadPool_;

    std::map<OmTileDrawer*, bool> drawersActive_;
    LockedInt32 numDrawersActive_;

    OmTileCacheImpl()
        : preFetcher_(new OmTilePreFetcher())
        , listener_(new OmTileCacheEventListener(this))
    {
        threadPool_.start(numThreads());
        numDrawersActive_.set(0);
    }

public:
    ~OmTileCacheImpl(){
        preFetcher_->Shutdown();
    }

    void RegisterDrawer(OmTileDrawer* d){
        drawersActive_[d] = false;
    }

    void UnRegisterDrawer(OmTileDrawer* d)
    {
        if(drawersActive_[d]){
            --numDrawersActive_;
        }
        drawersActive_.erase(d);
        runIdleThreadTask();
    }

    void SetDrawerActive(OmTileDrawer* d)
    {
        if(drawersActive_[d]){
            return;
        }

        drawersActive_[d] = true;
        ++numDrawersActive_;
        stopIdleThreadTask();
    }

    void SetDrawerDone(OmTileDrawer* d)
    {
        if(!drawersActive_[d]){
            return;
        }

        drawersActive_[d] = false;
        --numDrawersActive_;
        runIdleThreadTask();
    }

    void WidgetVisibilityChanged(OmTileDrawer* drawer, const bool visible)
    {
        if(visible){
            SetDrawerActive(drawer);

        }else{
            SetDrawerDone(drawer);
        }
    }

    void Get(OmTilePtr& tile, const OmTileCoord& key, const om::Blocking blocking)
    {
        if(isChannel(key)){
            getChanVol(key)->Get(tile, key, blocking);

        } else {
            getSegVol(key)->Get(tile, key, blocking);
        }
    }

    void GetDontQueue(OmTilePtr& tile, const OmTileCoord& key)
    {
        if(isChannel(key)){
            getChanVol(key)->GetDontQueue(tile, key);

        } else {
            getSegVol(key)->GetDontQueue(tile, key);
        }
    }

    void QueueUp(const OmTileCoord& key)
    {
        if(isChannel(key)){
            getChanVol(key)->QueueUp(key);

        } else {
            getSegVol(key)->QueueUp(key);
        }
    }

    void BlockingCreate(OmTilePtr& tile, const OmTileCoord& key)
    {
        if(isChannel(key)){
            getChanVol(key)->BlockingCreate(tile, key);

        } else {
            getSegVol(key)->BlockingCreate(tile, key);
        }
    }

    void Prefetch(const OmTileCoord& key, const int depthOffset)
    {
        if(isChannel(key)){
            getChanVol(key)->Prefetch(key, depthOffset);

        } else {
            getSegVol(key)->Prefetch(key, depthOffset);
        }
    }

    bool AreDrawersActive()
    {
        return numDrawersActive_.get() > 0 ||
            OmCacheManager::AmClosingDown() ||
            QApplication::mouseButtons() != Qt::NoButton;
    }

    void ClearAll()
    {
        ClearChannel();
        ClearSegmentation();
    }

    void ClearChannel()
    {
        std::vector<OmChannel*> vols = ChannelDataWrapper::GetPtrVec();
        FOR_EACH(iter, vols){
            (*iter)->TileCache()->Clear();
        }
    }

    void ClearSegmentation()
    {
        std::vector<OmSegmentation*> vols = SegmentationDataWrapper::GetPtrVec();
        FOR_EACH(iter, vols){
            (*iter)->TileCache()->Clear();
        }
    }

    void ClearFetchQueues()
    {
        ClearChannelFetchQueues();
        ClearSegmentationFetchQueues();
    }

    void ClearChannelFetchQueues()
    {
        std::vector<OmChannel*> vols = ChannelDataWrapper::GetPtrVec();
        FOR_EACH(iter, vols){
            (*iter)->TileCache()->ClearFetchQueue();
        }
    }

    void ClearSegmentationFetchQueues()
    {
        std::vector<OmSegmentation*> vols = SegmentationDataWrapper::GetPtrVec();
        FOR_EACH(iter, vols){
            (*iter)->TileCache()->ClearFetchQueue();
        }
    }

    OmTaskManager<OmTaskManagerContainerMipSorted>& ThreadPool(){
        return threadPool_;
    }

private:
    bool isChannel(const OmTileCoord& key){
        return CHANNEL == key.getVolume()->getVolumeType();
    }

    void runIdleThreadTask()
    {
        if(ZiARG_noTilePrefetch){
            return;
        }

        if(AreDrawersActive()){
            return;
        }

        std::list<OmTileDrawer*> drawers;
        FOR_EACH(iter, drawersActive_){
            drawers.push_back(iter->first);
        }

        preFetcher_->RunTasks(drawers);
    }

    void stopIdleThreadTask()
    {
        if(!AreDrawersActive()){
            return;
        }

        preFetcher_->ClearTasks();
    }

    int numThreads()
    {
        int num = zi::system::cpu_count;
        if(num > 8){
            return 4;
        }
        if(num < 2){
            return 2;
        }
        return num;
    }

    // TODO: refactor OmTile to contain real vol ptrs...

    inline OmTileCacheChannel* getChanVol(const OmTileCoord& key) const {
        return reinterpret_cast<OmChannel*>(key.getVolume())->TileCache();
    }

    inline OmTileCacheSegmentation* getSegVol(const OmTileCoord& key) const {
        return reinterpret_cast<OmSegmentation*>(key.getVolume())->TileCache();
    }

    friend class OmTileCache;
};

#endif
