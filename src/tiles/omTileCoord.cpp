#include "common/omDebug.h"
#include "tiles/omTileCoord.h"
#include "volume/mipVolume.h"
#include "viewGroup/omViewGroupState.h"

OmTileCoord::OmTileCoord()
    : OmTileCoordKey(-1,
                     om::globalCoord(),
                     NULL,
                     0,
                     NULL,
                     XY_VIEW,
                     SCC_NUMBER_OF_ENUMS)
{}

OmTileCoord::OmTileCoord(const int level, const om::globalCoord& dataCoord,
                         mipVolume* vol, const uint32_t freshness,
                         OmViewGroupState* vgs, const ViewType vt,
                         const ObjectType objType)
    : OmTileCoordKey(level,
                     dataCoord,
                     vol,
                     freshness,
                     vgs,
                     vt,
                     vgs->determineColorizationType(objType))
{}

OmTileCoord::OmTileCoord(const int level, const om::globalCoord& dataCoord,
                         mipVolume* vol, const uint32_t freshness,
                         OmViewGroupState* vgs, const ViewType vt,
                         const segmentColorCacheType segColorType)
    : OmTileCoordKey(level,
                     dataCoord,
                     vol,
                     freshness,
                     vgs,
                     vt,
                     segColorType)
{}

ObjectType OmTileCoord::getVolType() const {
    return getVolume()->getVolumeType();
}

std::ostream& operator<<(std::ostream &out, const OmTileCoord &c)
{
    out << "["
        << c.getLevel() << ", "
        << c.getCoord() << ", "
        << c.getVolume()->GetName() << ", "
        << c.getFreshness() << ", "
        << c.getViewType() << ", "
        << c.getSegmentColorCacheType()
        << "]";

    return out;
}
