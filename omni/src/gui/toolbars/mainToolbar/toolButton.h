#ifndef TOOL_BUTTON_H
#define TOOL_BUTTON_H

#include "common/omCommon.h"
#include "gui/widgets/omButton.h"

class ToolButton : public OmButton<QWidget> {
 public:
	ToolButton(QWidget*,
		   const QString & title, 
		   const QString & statusTip,
		   const OmToolMode tool,
		   const QString & iconPath);

	OmToolMode getToolMode(){ return mTool; }
 private:
	void doAction();
	OmToolMode mTool;
};

#endif
