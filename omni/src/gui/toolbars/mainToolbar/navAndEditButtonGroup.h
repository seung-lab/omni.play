#ifndef NAV_AND_EDIT_BUTTON_GROUP_H
#define NAV_AND_EDIT_BUTTON_GROUP_H

#include "gui/toolbars/mainToolbar/toolButton.h"

#include <QButtonGroup>
#include <QHash>

class NavAndEditButtonGroup : public QButtonGroup {
 Q_OBJECT
 public:
	explicit NavAndEditButtonGroup( QWidget * );
	virtual ~NavAndEditButtonGroup(){}

	void setReadOnlyWidgetsEnabled(const bool toBeEnabled);
	void setModifyWidgetsEnabled(const bool toBeEnabled);
	void setTool(const OmToolMode tool);

 private slots:
	void buttonWasClicked(const int id);

 private:
	int addButton(ToolButton* button);
	void addNavButton(ToolButton* button);
	void addModifyButton(ToolButton* button);
	void makeToolActive(ToolButton* button);

	QHash<int,ToolButton*> mAllToolsByID;

	QHash<OmToolMode,int> mNavToolIDsByToolType;
	QHash<OmToolMode,int> mModifyToolIDsByToolType;
};

#endif
