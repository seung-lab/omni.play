#ifndef DEND_TOOLBAR_BUTTONS_H
#define DEND_TOOLBAR_BUTTONS_H

#include "gui/widgets/omButton.h"

class SplitButton : public OmButton {
 public:
	SplitButton(MainWindow * mw, 
		    const QString & title, 
		    const QString & statusTip,
		    const bool isCheckable );
 private:
	void doAction();
};

#endif
