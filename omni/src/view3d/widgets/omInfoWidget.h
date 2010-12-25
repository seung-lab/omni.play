#ifndef OM_INFO_WIDGET_H
#define OM_INFO_WIDGET_H

#include "view3d/omView3dWidget.h"
#include <QFont>

class OmInfoWidget : public OmView3dWidget {

public:
	OmInfoWidget(OmView3d *view3d);
	virtual void Draw();

private:
	QFont mFont;

	void renderCameraText(int x, int y);
};

#endif
