#ifndef SPLIT_BUTTON_H
#define SPLIT_BUTTON_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"

class SplitButton : public OmButton<DendToolBar> {
 public:
	SplitButton(DendToolBar * mw, 
		    const QString & title, 
		    const QString & statusTip );

 private:
	void doAction();
};

#endif
