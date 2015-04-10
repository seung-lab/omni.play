#include "actions/io/omActionLogger.hpp"
#include "actions/omActionsImpl.h"
#include "events/details/managerImpl.hpp"
#include "project/omProjectGlobals.h"
#include "system/omStateManagerImpl.hpp"
#include "users/omUsers.h"
#include "utility/omRandColorFile.h"
#include "view2d/omView2dManagerImpl.hpp"

OmProjectGlobals::OmProjectGlobals(const om::file::Paths& paths)
    : randColorFile_(std::make_unique<om::utility::RandColorFile>(paths)),
      users_(std::make_unique<om::users>(paths)),
      v2dManagerImpl_(std::make_unique<OmView2dManagerImpl>()),
      stateMan_(std::make_unique<OmStateManagerImpl>()),
      actions_(std::make_unique<OmActionsImpl>()) {}

OmProjectGlobals::~OmProjectGlobals() {}

void OmProjectGlobals::Init() {
  fileReadThrottle_.set(4);

  users_->SetupFolders();
}
