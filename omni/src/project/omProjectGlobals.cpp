#include "actions/io/omActionLogger.hpp"
#include "actions/omActionsImpl.h"
#include "events/details/omEventManagerImpl.hpp"
#include "project/omProjectGlobals.h"
#include "system/omStateManagerImpl.hpp"
#include "utility/omRandColorFile.hpp"
#include "view2d/omView2dManagerImpl.hpp"

OmProjectGlobals::OmProjectGlobals()
    : randColorFile_(new OmRandColorFile())
    , v2dManagerImpl_(new OmView2dManagerImpl())
    , stateMan_(new OmStateManagerImpl())
    , eventMan_(new OmEventManagerImpl())
    , actions_(new OmActionsImpl())
    , actionLogger_(new OmActionLogger())
{
    randColorFile_->createOrLoad();
}

OmProjectGlobals::~OmProjectGlobals()
{}
