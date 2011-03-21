#include "common/omDebug.h"
#include "tiles/omTileCoord.h"
#include "volume/omMipVolume.h"
#include "viewGroup/omViewGroupState.h"

OmTileCoord::OmTileCoord()
    : OmTileCoordKey(-1,
                     DataCoord(),
                     NULL,
                     0,
                     NULL,
                     XY_VIEW,
                     SCC_NUMBER_OF_ENUMS)
{}

OmTileCoord::OmTileCoord(const int level, const DataCoord& dataCoord,
                         OmMipVolume* vol, const uint32_t freshness,
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

OmTileCoord::OmTileCoord(const int level, const DataCoord& dataCoord,
                         OmMipVolume* vol, const uint32_t freshness,
                         OmViewGroupState* vgs, const ViewType vt,
                         const OmSegmentColorCacheType segColorType)
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
        << c.getDataCoord() << ", "
        << c.getVolume()->GetName() << ", "
        << c.getFreshness() << ", "
        << c.getViewType() << ", "
        << c.getSegmentColorCacheType()
        << "]";

    return out;
}
