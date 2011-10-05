#include "actions/io/omActionLogger.hpp"
#include "actions/omActionsImpl.h"
#include "events/details/omEventManagerImpl.hpp"
#include "project/projectGlobals.h"
#include "system/omStateManagerImpl.hpp"
#include "users/omUsers.h"
#include "utility/omRandColorFile.hpp"
#include "view2d/omView2dManagerImpl.hpp"

projectGlobals::projectGlobals()
    : randColorFile_(new OmRandColorFile())
    , users_(new om::users())
    , v2dManagerImpl_(new OmView2dManagerImpl())
    , stateMan_(new OmStateManagerImpl())
    , eventMan_(new OmEventManagerImpl())
    , actions_(new OmActionsImpl())
    , actionLogger_(new OmActionLogger())
{}

projectGlobals::~projectGlobals()
{}

void projectGlobals::Init()
{
    fileReadThrottle_.set(4);

    randColorFile_->createOrLoad();
    actionLogger_->Init();
    users_->SetupFolders();
}
