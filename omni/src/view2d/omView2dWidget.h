#ifndef OM_VIEW_2D_WIDGET_H
#define OM_VIEW_2D_WIDGET_H

/*
 *	An interface for widgets belonging to the OmView2d viewing system.
 *
 *	Can call a View event if the widget needs to be redrawn.
 */

class OmCamera2d;
class OmView2d;

class OmView2dWidget {

public:
	OmView2dWidget(OmView2d *view2d) : mView2d(view2d) { };
	virtual void Draw() = 0;
	
protected:
	OmView2d * const mView2d;
};



#endif
