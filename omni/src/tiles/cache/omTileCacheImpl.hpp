#ifndef OM_TILE_CACHE_IMPL_HPP
#define OM_TILE_CACHE_IMPL_HPP

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

    void SetDrawerDone(OmTileDrawer* d)
    {
        if(!drawersActive_[d]){
            return;
        }

        drawersActive_[d] = false;
        --numDrawersActive_;
        runIdleThreadTask();
    }

    void WidgetVisibilityChanged(OmTileDrawer* drawer,
                                 const bool visible)
    {
        if(visible){
            setDrawerActive(drawer);
        }else{
            SetDrawerDone(drawer);
        }
    }

    void Get(OmTileDrawer* drawer,
             OmTilePtr& tile,
             const OmTileCoord& key,
             const om::Blocking blocking)
    {
        setDrawerActive(drawer);
        doGet(tile, key, blocking);
    }

    void GetDontQueue(OmTileDrawer* drawer,
                      OmTilePtr& tile,
                      const OmTileCoord& key)
    {
        setDrawerActive(drawer);
        doGetDontQueue(tile, key);
    }

    void BlockingCreate(OmTileDrawer*,
                        OmTilePtr& tile,
                        const OmTileCoord& key)
    {
        cacheSegmentation_->BlockingCreate(tile, key);
    }

    void Prefetch(const OmTileCoord& key)
    {
        if(isChannel(key)){
            cacheChannel_->Prefetch(key);
        } else {
            cacheSegmentation_->Prefetch(key);
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
        cacheChannel_->Clear();
        cacheSegmentation_->Clear();
    }

    void ClearChannel(){
        cacheChannel_->Clear();
    }

    void ClearSegmentation(){
        cacheSegmentation_->Clear();
    }

    void ClearFetchQueues()
    {
        cacheChannel_->ClearFetchQueue();
        cacheSegmentation_->ClearFetchQueue();
    }

private:
    boost::scoped_ptr<OmTileCacheChannel> cacheChannel_;
    boost::scoped_ptr<OmTileCacheSegmentation> cacheSegmentation_;
    boost::scoped_ptr<OmTilePreFetcher> preFetcher_;
    boost::scoped_ptr<OmTileCacheEventListener> listener_;

    std::map<OmTileDrawer*, bool> drawersActive_;
    LockedInt32 numDrawersActive_;

    OmTileCacheImpl()
        : cacheChannel_(new OmTileCacheChannel())
        , cacheSegmentation_(new OmTileCacheSegmentation())
        , preFetcher_(new OmTilePreFetcher())
        , listener_(new OmTileCacheEventListener(this))
    {
        numDrawersActive_.set(0);
    }

    bool isChannel(const OmTileCoord& key){
        return CHANNEL == key.getVolume()->getVolumeType();
    }

    void setDrawerActive(OmTileDrawer* d)
    {
        if(drawersActive_[d]){
            return;
        }

        drawersActive_[d] = true;
        ++numDrawersActive_;
        stopIdleThreadTask();
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

    void doGet(OmTilePtr& tile,
               const OmTileCoord& key,
               const om::Blocking blocking)
    {
        if(isChannel(key)){
            cacheChannel_->Get(tile, key, blocking);

        } else {
            cacheSegmentation_->Get(tile, key, blocking);
        }
    }

    void doGetDontQueue(OmTilePtr& tile,
                        const OmTileCoord& key)
    {
        if(isChannel(key)){
            cacheChannel_->GetDontQueue(tile, key);

        } else {
            cacheSegmentation_->GetDontQueue(tile, key);
        }
    }

    friend class OmTileCache;
};

#endif
