#include "common/omDebug.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/toolbars/dendToolbar/showValidatedButton.h"
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
    //debug(valid, "DendToolBar::mapColors(%i)\n", isChecked());
    // Using !(not) because check happens after this fuction.
    mParent->getViewGroupState()->SetShowValidMode(!isChecked(), mParent->isShowValidChecked());
}
