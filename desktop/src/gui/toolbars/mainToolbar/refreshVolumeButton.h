#pragma once

#include "gui/widgets/omButton.hpp"

class MainWindow;

class VolumeRefreshButton : public OmButton<MainWindow> {

 public:
	VolumeRefreshButton(MainWindow * mw);

 private:
	void doAction();
};

