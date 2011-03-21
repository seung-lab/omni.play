#ifndef OM_TILE_DUMPER_HPP
#define OM_TILE_DUMPER_HPP

#include "common/omCommon.h"

class OmMipVolume;
class OmViewGroupState;

class OmTileDumper{
public:
    OmTileDumper(OmMipVolume* vol, const QString& dumpfile,
                 OmViewGroupState* vgs);

    void DumpTiles();

private:
    OmMipVolume *const vol_;
    const QString dumpfile_;
    OmViewGroupState *const vgs_;

    void saveTile(QDataStream&, const int,
                  const int, const int, const int,
                  const ViewType);
};

#endif
