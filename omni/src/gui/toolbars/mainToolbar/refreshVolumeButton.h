#ifndef VOLUME_REFRESH_BUTTON_H
#define VOLUME_REFRESH_BUTTON_H

#include "gui/widgets/omButton.h"

class MainWindow;

class VolumeRefreshButton : public OmButton<MainWindow> {

 public:
	VolumeRefreshButton(MainWindow * mw);

 private:
	void doAction();
};

#endif
