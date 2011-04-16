#ifndef OM_TILE_CACHE_H
#define OM_TILE_CACHE_H

#include "common/om.hpp"
#include "common/omCommon.h"
#include "tiles/omTileTypes.hpp"
#include "zi/omUtility.h"

class OmTileCoord;
class OmTileCacheImpl;
class OmTileDrawer;

class OmTileCache : private om::singletonBase<OmTileCache>{
private:
    boost::scoped_ptr<OmTileCacheImpl> impl_;

public:
    static void Delete();
    static void Reset();

    static void RegisterDrawer(OmTileDrawer*);
    static void UnRegisterDrawer(OmTileDrawer*);
    static void SetDrawerActive(OmTileDrawer*);
    static void SetDrawerDone(OmTileDrawer*);
    static bool AreDrawersActive();
    static void WidgetVisibilityChanged(OmTileDrawer* drawer,
                                        const bool visible);

    static void Get(OmTilePtr& tile, const OmTileCoord& key, const om::Blocking blocking);

    static void QueueUp(const OmTileCoord& key);

    static void GetDontQueue(OmTilePtr& tile, const OmTileCoord& key);

    static void BlockingCreate(OmTilePtr& tile, const OmTileCoord& key);

    static void Prefetch(const OmTileCoord& key);

    static void ClearAll();
    static void ClearChannel();
    static void ClearSegmentation();

private:
    OmTileCache();
    ~OmTileCache();

    friend class zi::singleton<OmTileCache>;
};

#endif
