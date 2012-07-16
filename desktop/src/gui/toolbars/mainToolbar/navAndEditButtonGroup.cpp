#include "events/omEvents.h"
#include "gui/toolbars/mainToolbar/navAndEditButtonGroup.h"
#include "gui/toolbars/mainToolbar/toolButton.h"
#include "gui/toolbars/mainToolbar/toolButton.h"
#include "system/omConnect.hpp"
#include "system/omStateManager.h"

NavAndEditButtonGroup::NavAndEditButtonGroup(QWidget* parent)
    : QButtonGroup(parent)
{
    addNavButton(new ToolButton(parent, "Crosshair",
                                "Recenter Dispaly", om::tool::CROSSHAIR,
                                ":/toolbars/mainToolbar/icons/1277962376_target.png"));

    // addNavButton(new ToolButton(parent, "Zoom",
    //                             "Zoom", om::tool::ZOOM,
    //                             ":/toolbars/mainToolbar/icons/1278008421_search.png"));

    addNavButton(new ToolButton(parent, "Pan",
                                "Move Image", om::tool::PAN,
                                ":/toolbars/mainToolbar/icons/1277962397_cursor_hand.png"));

    addNavButton(new ToolButton(parent, "Select",
                                "Select Object", om::tool::SELECT,
                                ":/toolbars/mainToolbar/icons/1278008858_cursor_arrow.png"));

    addModifyButton(new ToolButton(parent, "Brush",
                                   "Paint", om::tool::PAINT,
                                   ":/toolbars/mainToolbar/icons/1277962300_paint.png"));

    addModifyButton(new ToolButton(parent, "Eraser",
                                   "Paint Black Voxel", om::tool::ERASE,
                                   ":/toolbars/mainToolbar/icons/1277962354_package-purge.png"));

    addModifyButton(new ToolButton(parent, "Fill",
                                   "Paint Can", om::tool::FILL,
                                   ":/toolbars/mainToolbar/icons/1278015539_color_fill.png"));

    addModifyButton(new ToolButton(parent, "Landmarks",
                                   "Landmarks", om::tool::LANDMARK,
                                   ":/toolbars/mainToolbar/icons/1308021634_keditbookmarks.png"));

    addModifyButton(new ToolButton(parent, "Cut",
                                   "Cut", om::tool::CUT,
                                   ":/toolbars/mainToolbar/icons/1308183310_Scissors.png"));
    
    addModifyButton(new ToolButton(parent, "Annotate",
                                    "Annotate", om::tool::ANNOTATE,
                                   ":/toolbars/mainToolbar/icons/kcmfontinst.png"));

    addNavButton(new ToolButton(parent, "Kalina",
                                "Do Crazy Stuff", om::tool::KALINA,
                                ":/toolbars/mainToolbar/icons/1277962552_highlight.png"));

    om::connect( this, SIGNAL(buttonClicked(int)),
                 this, SLOT(buttonWasClicked(int)));

    om::connect(this, SIGNAL(signalSetTool(om::tool::mode)),
                this, SLOT(findAndSetTool(om::tool::mode)));
}

int NavAndEditButtonGroup::addButton(ToolButton* button)
{
    QButtonGroup::addButton(button);
    const int id = QButtonGroup::id(button);
    allToolsByID_[id] = button;
    return id;
}

void NavAndEditButtonGroup::addNavButton(ToolButton* button)
{
    const int id = addButton(button);
    navToolIDsByToolMode_[button->getToolMode()] = id;
}

void NavAndEditButtonGroup::addModifyButton(ToolButton* button)
{
    const int id = addButton(button);
    modifyToolIDsByToolMode_[button->getToolMode()] = id;
}

void NavAndEditButtonGroup::buttonWasClicked(const int id)
{
    ToolButton* button = allToolsByID_.at(id);
    makeToolActive(button);
}

void NavAndEditButtonGroup::makeToolActive(ToolButton* button){
    OmStateManager::SetToolModeAndSendEvent(button->getToolMode());
}

void NavAndEditButtonGroup::SetReadOnlyWidgetsEnabled(const bool toBeEnabled)
{
    FOR_EACH(iter, navToolIDsByToolMode_ )
    {
        ToolButton* button = allToolsByID_.at(iter->second);

        button->setEnabled(toBeEnabled);

        if(om::tool::PAN == button->getToolMode())
        {
            if(toBeEnabled)
            {
                button->setChecked(true);
                makeToolActive(button);
            }
        }
    }
}

void NavAndEditButtonGroup::SetModifyWidgetsEnabled(const bool toBeEnabled)
{
    FOR_EACH(iter, modifyToolIDsByToolMode_ )
    {
        ToolButton* button = allToolsByID_.at(iter->second);
        button->setEnabled(toBeEnabled);
    }
}

void NavAndEditButtonGroup::SetTool(const om::tool::mode tool){
    signalSetTool(tool);
}

void NavAndEditButtonGroup::findAndSetTool(const om::tool::mode tool)
{
    int id = 0;

    if(navToolIDsByToolMode_.count(tool)){
        id = navToolIDsByToolMode_.at(tool);

    } else if(modifyToolIDsByToolMode_.count(tool)){
        id = modifyToolIDsByToolMode_.at(tool);

    } else {
        throw OmArgException("tool not found");
    }

    ToolButton * button = allToolsByID_.at(id);
    button->setChecked(true);
    makeToolActive(button);
}
