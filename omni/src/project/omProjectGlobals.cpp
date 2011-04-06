#include "actions/io/omActionLogger.hpp"
#include "actions/omActionsImpl.h"
#include "events/details/omEventManagerImpl.hpp"
#include "project/omProjectGlobals.h"
#include "system/omStateManagerImpl.hpp"
#include "tiles/omTilePool.hpp"
#include "utility/omRandColorFile.hpp"
#include "view2d/omView2dManagerImpl.hpp"

OmProjectGlobals::OmProjectGlobals()
    : randColorFile_(new OmRandColorFile())
    , v2dManagerImpl_(new OmView2dManagerImpl())
    , stateMan_(new OmStateManagerImpl())
    , eventMan_(new OmEventManagerImpl())
    , actions_(new OmActionsImpl())
    , actionLogger_(new OmActionLogger())
    , tilePoolUint8_(new OmTilePool<uint8_t>(20, 128*128))
    , tilePoolARGB_(new OmTilePool<OmColorARGB>(20, 128*128))
{
    randColorFile_->createOrLoad();
}

OmProjectGlobals::~OmProjectGlobals()
{}
