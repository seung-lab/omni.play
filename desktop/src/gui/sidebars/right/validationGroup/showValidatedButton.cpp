#include "common/logging.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/right/validationGroup/showValidatedButton.h"
#include "system/omStateManager.h"
#include "viewGroup/omViewGroupState.h"

ShowValidatedButton::ShowValidatedButton(ValidationGroup * d)
    : OmButton<ValidationGroup>( d,
                                 "Show Validated",
                                 "Validated object mode",
                                 true)
{
}

void ShowValidatedButton::doAction()
{
    //debug(valid, "om::sidebars::rightImpl::mapColors(%i)\n", isChecked());
    // Using !(not) because check happens after this fuction.
    mParent->GetViewGroupState()->SetShowValidMode(!isChecked(), mParent->isShowValidChecked());
}
