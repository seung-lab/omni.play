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

void OmTileCache::Prefetch(const OmTileCoord& key, const int depthOffset){
    impl()->Prefetch(key, depthOffset);
}

void OmTileCache::QueueUp(const OmTileCoord& key)
{
    impl()->QueueUp(key);
}

void OmTileCache::Get(OmTilePtr& tile, const OmTileCoord& key, const om::common::Blocking blocking)
{
    impl()->Get(tile, key, blocking);
}

void OmTileCache::GetDontQueue(OmTilePtr& tile, const OmTileCoord& key)
{
    impl()->GetDontQueue(tile, key);
}

void OmTileCache::BlockingCreate(OmTilePtr& tile, const OmTileCoord& key)
{
    impl()->BlockingCreate(tile, key);
}

void OmTileCache::RegisterDrawer(OmTileDrawer* d){
    impl()->RegisterDrawer(d);
}

void OmTileCache::UnRegisterDrawer(OmTileDrawer* d){
    impl()->UnRegisterDrawer(d);
}

void OmTileCache::SetDrawerDone(OmTileDrawer* d){
    impl()->SetDrawerDone(d);
}

void OmTileCache::SetDrawerActive(OmTileDrawer* d){
    impl()->SetDrawerActive(d);
}

void OmTileCache::WidgetVisibilityChanged(OmTileDrawer* drawer, const bool visible){
    impl()->WidgetVisibilityChanged(drawer, visible);
}

bool OmTileCache::AreDrawersActive(){
    return impl()->AreDrawersActive();
}

void OmTileCache::ClearAll(){
    impl()->ClearAll();
}

void OmTileCache::ClearSegmentation(){
    impl()->ClearSegmentation();
}

void OmTileCache::ClearChannel(){
    impl()->ClearChannel();
}

OmTileCacheThreadPool& OmTileCache::ThreadPool(){
    return impl()->ThreadPool();
}
