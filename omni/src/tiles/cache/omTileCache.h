#ifndef OM_TILE_CACHE_H
#define OM_TILE_CACHE_H

#include "common/om.hpp"
#include "common/omCommon.h"
#include "zi/omUtility.h"

class OmTileCoord;
class OmTileCacheImpl;
class OmTileDrawer;

class OmTileCache : private om::singletonBase<OmTileCache>{
private:
	boost::shared_ptr<OmTileCacheImpl> impl_;

public:
	static void Reset();

	static void RegisterDrawer(OmTileDrawer*);
	static void UnRegisterDrawer(OmTileDrawer*);
	static void SetDrawerDone(OmTileDrawer*);
	static bool AreDrawersActive();
	static void WidgetVisibilityChanged(boost::shared_ptr<OmTileDrawer> drawer,
										const bool visible);

	static void Get(OmTileDrawer* drawer,
					OmTilePtr& tile,
					const OmTileCoord& key,
					const om::Blocking blocking);

	static void Prefetch(const OmTileCoord& key);

	static void RemoveSpaceCoord(const SpaceCoord & coord);
	static void Clear();
	static void ClearChannel();

private:
	OmTileCache();
	~OmTileCache(){}

	static void doGet(OmTilePtr& tile,
					  const OmTileCoord& key,
					  const om::Blocking blocking);

	friend class OmTileDumper; // access doGet(...)

	friend class zi::singleton<OmTileCache>;
};

#endif
