#include "common/omDebug.h"
#include "tiles/tileCoord.h"
#include "volume/mipVolume.h"
#include "viewGroup/omViewGroupState.h"

tileCoord::tileCoord()
    : tileCoordKey(-1,
                     om::globalCoord(),
                     NULL,
                     0,
                     NULL,
                     XY_VIEW,
                     SCC_NUMBER_OF_ENUMS)
{}

tileCoord::tileCoord(const int level, const om::globalCoord& dataCoord,
                         mipVolume* vol, const uint32_t freshness,
                         OmViewGroupState* vgs, const ViewType vt,
                         const ObjectType objType)
    : tileCoordKey(level,
                     dataCoord,
                     vol,
                     freshness,
                     vgs,
                     vt,
                     vgs->determineColorizationType(objType))
{}

tileCoord::tileCoord(const int level, const om::globalCoord& dataCoord,
                         mipVolume* vol, const uint32_t freshness,
                         OmViewGroupState* vgs, const ViewType vt,
                         const segmentColorCacheType segColorType)
    : tileCoordKey(level,
                     dataCoord,
                     vol,
                     freshness,
                     vgs,
                     vt,
                     segColorType)
{}

ObjectType tileCoord::getVolType() const {
    return getVolume()->getVolumeType();
}

std::ostream& operator<<(std::ostream &out, const tileCoord &c)
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
