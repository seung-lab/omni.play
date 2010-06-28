#ifndef DEND_TOOLBAR_BUTTONS_H
#define DEND_TOOLBAR_BUTTONS_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar.h"

class SplitButton : public OmButton<DendToolBar> {
 public:
	SplitButton(DendToolBar * mw, 
		    const QString & title, 
		    const QString & statusTip,
		    const bool isCheckable );
 private:
	void doAction();
};

#endif
