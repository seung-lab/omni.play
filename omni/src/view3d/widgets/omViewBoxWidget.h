#ifndef OM_VIEW_BOX_WIDGET_H
#define OM_VIEW_BOX_WIDGET_H

#include "view3d/omCamera.h"
#include "view3d/omView3dWidget.h"

class OmViewBoxWidget : public OmView3dWidget {

public:
	OmViewBoxWidget(OmView3d *view3d) : OmView3dWidget(view3d) { };
	virtual void Draw();
	
private:
	void drawRectangle(SpaceCoord v0, SpaceCoord v1, SpaceCoord v2, SpaceCoord v3);
	void drawSlice(ViewType plane, Vector2 < float >min, Vector2 < float >max, float depth);
	void drawLines(SpaceCoord depth);
};



#endif
