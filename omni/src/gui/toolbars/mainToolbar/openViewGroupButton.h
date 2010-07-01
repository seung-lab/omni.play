#ifndef OPEN_VIEW_GROUP_BUTTON_H
#define OPEN_VIEW_GROUP_BUTTON_H

#include "gui/widgets/omButton.h"

class MainWindow;

class OpenViewGroupButton : public OmButton<MainWindow> {
 public:
	OpenViewGroupButton(MainWindow * mw);
 private:
	void doAction();
};

#endif
