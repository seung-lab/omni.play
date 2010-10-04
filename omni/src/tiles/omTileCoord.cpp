#include "common/omDebug.h"
#include "tiles/omTileCoord.h"
#include "volume/omMipVolume.h"
#include "viewGroup/omViewGroupState.h"

OmTileCoord::OmTileCoord()
	: OmTileCoordKey(-1,
			 SpaceCoord(),
			 NULL,
			 0,
			 NULL,
			 XY_VIEW,
			 SCC_NUMBER_OF_ENUMS) {}

OmTileCoord::OmTileCoord(const int level, const SpaceCoord& spaceCoord,
			 OmMipVolume* vol, const uint32_t freshness,
			 OmViewGroupState* vgs, const ViewType vt)
	: OmTileCoordKey(level,
			 spaceCoord,
			 vol,
			 freshness,
			 vgs,
			 vt,
			 vgs->determineColorizationType(vol->getVolumeType()))
{}

ObjectType OmTileCoord::getVolType() const {
	return getVolume()->getVolumeType();
}

std::ostream& operator<<(std::ostream &out, const OmTileCoord &c) {
	out << "["
	    << c.getLevel() << ", "
	    << c.getSpaceCoord() << ", "
	    << c.getVolume()->GetName() << ", "
	    << c.getFreshness() << ", "
	    << c.getViewType() << ", "
	    << c.getSegmentColorCacheType()
	    << "]";

	return out;
}
