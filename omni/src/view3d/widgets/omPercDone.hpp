#ifndef OM_PERC_DONE_WIDGET_HPP
#define OM_PERC_DONE_WIDGET_HPP

#include "view3d/omView3dWidget.h"
#include "common/omGl.h"
#include "system/omStateManager.h"
#include "view3d/omCamera.h"
#include "view3d/omView3d.h"
#include "view3d/widgets/omInfoWidget.h"

#include <QFont>

class OmPercDone : public OmView3dWidget {
public:
	OmPercDone(OmView3d *view3d)
		: OmView3dWidget(view3d)
	{
		//set font properties
		mFont = QFont("Helvetica", 12);
		mFont.setWeight(QFont::Bold);
		mFont.setItalic(true);
	}

	virtual void Draw()
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		//glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
		glDisable(GL_LIGHTING);	// need to disable lighting for proper text color

		static const float TEXT_COLOR[4] = { 1, 1, 1, 0.8 };
		glColor4fv(TEXT_COLOR);	// set text color

		showPercVolDone();

		glPopAttrib();
	}

private:
	QFont mFont;

	void showPercVolDone()
	{
		const std::list<std::pair<float,float> >& percVolDone =
			mView3d->PercVolDone();

		if(percVolDone.empty()){
			return;
		}

		float num = 0;
		float denom = 0;
		FOR_EACH(iter, percVolDone){
			num += iter->first;
			denom += iter->second;
		}

		const float val = num / denom;

		const QString str = QString("%1%")
			.arg(QString::number(val*100., 'f', 2));

		const int x = mView3d->width() - 75;
		const int y = mView3d->height() - 15;

		mView3d->renderText(x, y, qPrintable(str), mFont);
	}
};

#endif
