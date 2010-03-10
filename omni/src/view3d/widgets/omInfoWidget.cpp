
#include "omInfoWidget.h"
#include "view3d/omCamera.h"
#include "view3d/omView3d.h"

#include "system/omStateManager.h"
#include "common/omGl.h"

#define DEBUG 0

static const float TEXT_COLOR[4] = { 1, 1, 1, 0.8 };

////////////////////////////////////////
///////         Utility Functions Prototypes
//void drawString(const char *str, int x, int y, float color[4], void *font);
//void printf2str(string &str, const char *fmt, ...);

/////////////////////////////////
///////
///////          Example Class
///////

 OmInfoWidget::OmInfoWidget(OmView3d * view3d):OmView3dWidget(view3d)
{

	//set font properties
	mFont = QFont("Helvetica", 12);
	mFont.setWeight(QFont::Bold);
	mFont.setItalic(true);
};

/////////////////////////////////
///////          Draw Methods

void OmInfoWidget::Draw()
{

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);	// need to disable lighting for proper text color

	glColor4fv(TEXT_COLOR);	// set text color
	int height = mView3d->height();
	RenderCameraText(10, height - 30);

	glPopAttrib();
}

void OmInfoWidget::RenderCameraText(int x, int y)
{
	string str;

	SpaceCoord focus = mView3d->GetCamera().GetFocus();
	float distance = mView3d->GetCamera().GetDistance();

	printf2str(str, "Focus: ( %.2f, %.2f, %.2f )", focus.x, focus.y, focus.z);
	mView3d->renderText(x, y, str.c_str(), mFont);

	printf2str(str, "Distance: %.2f", distance);
	mView3d->renderText(x, y + 15, str.c_str(), mFont);
}

/*
void drawString(const char *str, int x, int y, float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	
    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position
	
    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }
	
    glEnable(GL_LIGHTING);
    glPopAttrib();
}
*/
