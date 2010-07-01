#include "gui/toolbars/mainToolbar/navAndEditButtonGroup.h"
#include "gui/toolbars/mainToolbar/toolButton.h"
#include "system/omEvents.h"
#include "system/omStateManager.h"

NavAndEditButtonGroup::NavAndEditButtonGroup(QWidget * parent)
	: QButtonGroup(parent)
{
	addNavButton(new ToolButton(parent, "Select", 
				    "Select Object", SELECT_MODE,
				    ":/toolbars/mainToolbar/icons/1278008858_cursor_arrow.png"));
	addNavButton(new ToolButton(parent, "Crosshair", 
				    "Recenter Dispaly", CROSSHAIR_MODE,
				    ":/toolbars/mainToolbar/icons/1277962376_target.png"));
	addNavButton(new ToolButton(parent, "Pan", 
				    "Move Image", PAN_MODE,
				    ":/toolbars/mainToolbar/icons/1277962397_cursor_hand.png"));
	addNavButton(new ToolButton(parent, "Zoom", 
				    "Zoom", ZOOM_MODE,
				    ":/toolbars/mainToolbar/icons/1278008421_search.png"));

	//	addButton(new ToolButton(parent, "Voxelize", 
	//			 "Voxelize", VOXELIZE_MODE));

	addModifyButton(new ToolButton(parent, "Brush", 
				       "Paint", ADD_VOXEL_MODE,
				       ":/toolbars/mainToolbar/icons/1277962300_paint.png"));
	addModifyButton(new ToolButton(parent, "Eraser", 
				       "Paint Black Voxel", SUBTRACT_VOXEL_MODE,
				       ":/toolbars/mainToolbar/icons/1277962354_package-purge.png"));
	addModifyButton(new ToolButton(parent, "Fill", 
				       "Paint Can", FILL_MODE,
				       ":/toolbars/mainToolbar/icons/1277962298_krita_kra.png"));

	connect( this, SIGNAL(buttonClicked(int)), 
		 this, SLOT(buttonWasClicked(int)),
		 Qt::DirectConnection );
}

int NavAndEditButtonGroup::addButton(ToolButton* button)
{
	QButtonGroup::addButton(button);
	const int id = QButtonGroup::id(button);
	mAllToolsByID.insert(id, button);
	return id;
}

void NavAndEditButtonGroup::addNavButton(ToolButton* button)
{
	const int id = addButton(button);
	mNavToolIDsByToolType.insert(button->getToolMode(), id);
}

void NavAndEditButtonGroup::addModifyButton(ToolButton* button)
{
	const int id = addButton(button);
	mModifyToolIDsByToolType.insert(button->getToolMode(), id);
}

void NavAndEditButtonGroup::buttonWasClicked(const int id)
{
	ToolButton * button = mAllToolsByID.value(id);
	assert(button);
	makeToolActive(button);
}

void NavAndEditButtonGroup::makeToolActive(ToolButton* button)
{
	OmStateManager::SetToolModeAndSendEvent(button->getToolMode());
}

void NavAndEditButtonGroup::setReadOnlyWidgetsEnabled(const bool toBeEnabled)
{
	foreach( int id, mNavToolIDsByToolType ){
		ToolButton * button = mAllToolsByID.value(id);
		button->setEnabled(toBeEnabled);
		if(PAN_MODE == button->getToolMode()){
			if(toBeEnabled){
				button->setChecked(true);
				makeToolActive(button);
			}
		}
	}
}

void NavAndEditButtonGroup::setModifyWidgetsEnabled(const bool toBeEnabled)
{
	foreach( int id, mModifyToolIDsByToolType ){
		ToolButton * button = mAllToolsByID.value(id);
		button->setEnabled(toBeEnabled);
	}
}
