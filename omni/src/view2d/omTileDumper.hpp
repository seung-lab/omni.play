#ifndef OM_TILE_DUMPER_HPP
#define OM_TILE_DUMPER_HPP

#include "common/omCommon.h"

class OmViewGroupState;

class OmTileDumper{
public:
	static void DumpTiles(OmId vol, ObjectType type, const QString dumpfile, OmViewGroupState * vgs);
};

#endif
