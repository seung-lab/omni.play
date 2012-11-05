#include "common/omDebug.h"
#include "tiles/omTileCoord.h"
#include "volume/omMipVolume.h"
#include "viewGroup/omViewGroupState.h"
#include "view2d/omView2dConverters.hpp"

OmTileCoord::OmTileCoord()
    : OmTileCoordKey(om::coords::Chunk(),
                     XY_VIEW,
                     -1,
                     NULL,
                     -1,
                     NULL,
                     om::common::SCC_NUMBER_OF_ENUMS)
{}

OmTileCoord::OmTileCoord(const om::coords::Chunk& cc, ViewType view, uint8_t depth,
                         OmMipVolume* vol, uint32_t freshness,
                         OmViewGroupState* vgs, om::common::SegmentColorCacheType segColorType)
    : OmTileCoordKey(cc,
                     view,
                     depth,
                     vol,
                     freshness,
                     vgs,
                     segColorType)
{}

OmTileCoord::OmTileCoord(const om::coords::Chunk& cc, ViewType view, uint8_t depth,
                         OmMipVolume* vol, uint32_t freshness,
                         OmViewGroupState* vgs, om::common::ObjectType objType)
    : OmTileCoordKey(cc,
                     view,
                     depth,
                     vol,
                     freshness,
                     vgs,
                     vgs->determineColorizationType(objType))
{}

OmTileCoord OmTileCoord::Downsample() const
{
	int newDepth = (getDepth() +
		(OmView2dConverters::GetViewTypeDepth(getCoord().Coordinate, getViewType()) % 2) * 128) / 2;
	return OmTileCoord(getCoord().ParentCoord(),
	                   getViewType(),
	                   newDepth,
	                   getVolume(),
	                   getFreshness(),
	                   getViewGroupState(),
	                   getSegmentColorCacheType());
}


std::ostream& operator<<(std::ostream &out, const OmTileCoord &c)
{
    out << "["
        << c.getCoord() << ", "
        << c.getViewType() << ", "
        << int(c.getDepth()) << ", "
        << c.getVolume()->GetName() << ", "
        << c.getFreshness() << ", "
        << c.getSegmentColorCacheType()
        << "]";

    return out;
}
