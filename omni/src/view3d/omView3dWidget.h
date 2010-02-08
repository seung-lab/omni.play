#ifndef OM_VIEW_3D_WIDGET_H
#define OM_VIEW_3D_WIDGET_H

/*
 *	An interface for widgets belonging to the OmView3d viewing system.
 *
 *	Can call a View3d event if the widget needs to be redrawn.
 */


class OmCamera;
class OmView3d;

class OmView3dWidget {

public:
	OmView3dWidget(OmView3d *view3d) : mView3d(view3d) { };
	virtual void Draw() = 0;
	
protected:
	OmView3d * const mView3d;
};



#endif
