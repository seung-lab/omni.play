#ifndef OM_TILE_COORD_H
#define OM_TILE_COORD_H

#include "common/omCommon.h"
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

class OmMipVolume;
class OmMipChunkCoord;
class OmViewGroupState;

// Level, Coordinate, OmMipVolume*, freshness, OmViewGroupState*,
//  ViewType, OmSegmentColorCacheType
typedef boost::tuple<int, SpaceCoord,
		     OmMipVolume*, uint32_t,
		     OmViewGroupState*, ViewType,
		     OmSegmentColorCacheType> OmTileCoordKey;

class OmTileCoord : public OmTileCoordKey {
public:
	OmTileCoord();
	OmTileCoord(const int, const SpaceCoord&,
		    OmMipVolume*, const uint32_t,
		    OmViewGroupState*, const ViewType);

	ObjectType getVolType() const;
	OmMipChunkCoord getOmMipChunkCoord() const;

	int getLevel() const {
		return this->get<0>();
	}
	const SpaceCoord& getSpaceCoord() const {
		return this->get<1>();
	}
	OmMipVolume* getVolume() const {
		return this->get<2>();
	}
	uint32_t getFreshness() const {
		return this->get<3>();
	}
	OmViewGroupState* getViewGroupState() const {
		return this->get<4>();
	}
	ViewType getViewType() const {
		return this->get<5>();
	}
	OmSegmentColorCacheType getSegmentColorCacheType() const {
		return this->get<6>();
	}

	friend std::ostream& operator<<(std::ostream &, const OmTileCoord &);
};

#endif
