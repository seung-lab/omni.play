#include "tiles/cache/omTileCache.h"
#include "tiles/cache/omTileCacheImpl.hpp"

OmTileCache::OmTileCache()
{}

OmTileCache::~OmTileCache()
{}

void OmTileCache::Delete(){
    instance().impl_.reset();
}

void OmTileCache::Reset(){
    instance().impl_.reset(new OmTileCacheImpl());
}

void OmTileCache::Prefetch(const OmTileCoord& key){
    instance().impl_->Prefetch(key);
}

void OmTileCache::Get(OmTileDrawer* drawer,
                      OmTilePtr& tile,
                      const OmTileCoord& key,
                      const om::Blocking blocking)
{
    instance().impl_->Get(drawer, tile, key, blocking);
}

void OmTileCache::BlockingCreate(OmTileDrawer* drawer,
                                 OmTilePtr& tile,
                                 const OmTileCoord& key)
{
    instance().impl_->BlockingCreate(drawer, tile, key);
}

void OmTileCache::doGet(OmTilePtr& tile,
                        const OmTileCoord& key,
                        const om::Blocking blocking)
{
    instance().impl_->doGet(tile, key, blocking);
}

void OmTileCache::RegisterDrawer(OmTileDrawer* d){
    instance().impl_->RegisterDrawer(d);
}

void OmTileCache::UnRegisterDrawer(OmTileDrawer* d){
    instance().impl_->UnRegisterDrawer(d);
}

void OmTileCache::SetDrawerDone(OmTileDrawer* d){
    instance().impl_->SetDrawerDone(d);
}

void OmTileCache::WidgetVisibilityChanged(OmTileDrawer* drawer,
                                          const bool visible){
    instance().impl_->WidgetVisibilityChanged(drawer, visible);
}

bool OmTileCache::AreDrawersActive(){
    return instance().impl_->AreDrawersActive();
}

void OmTileCache::ClearAll(){
    instance().impl_->ClearAll();
}

void OmTileCache::ClearSegmentation(){
    instance().impl_->ClearSegmentation();
}

void OmTileCache::ClearChannel(){
    instance().impl_->ClearChannel();
}
