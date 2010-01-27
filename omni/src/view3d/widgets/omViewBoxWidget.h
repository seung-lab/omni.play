#ifndef OM_VIEW_BOX_WIDGET_H
#define OM_VIEW_BOX_WIDGET_H

/*
 *
 *
 */

#include "view3d/omView3dWidget.h"

class OmViewBoxWidget:public OmView3dWidget {

 public:
	OmViewBoxWidget(OmView3d * view3d):OmView3dWidget(view3d) {
	};
	virtual void Draw();

 private:

};

#endif
