#include "utility/glInclude.h"
#include "system/omStateManager.h"
#include "view3d/omCamera.h"
#include "view3d/omView3d.h"
#include "view3d/widgets/omInfoWidget.h"

static const float TEXT_COLOR[4] = { 1, 1, 1, 0.8 };

OmInfoWidget::OmInfoWidget(OmView3d * view3d)
    : OmView3dWidget(view3d)
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
    glDisable(GL_LIGHTING); // need to disable lighting for proper text color

    glColor4fv(TEXT_COLOR); // set text color
    const int height = View3dPtr()->height();
    renderCameraText(10, height - 30);

    glPopAttrib();
}

void OmInfoWidget::renderCameraText(int x, int y)
{
    std::string str;

    const Vector3f focus = View3dPtr()->GetCamera().GetFocus();

    str = QString("Focus: ( %1, %2, %3 )")
        .arg(focus.x)
        .arg(focus.y)
        .arg(focus.z).toStdString();
    View3dPtr()->renderText(x, y, str.c_str(), mFont);

    const float distance = View3dPtr()->GetCamera().GetDistance();
    str = QString("Distance: %1")
        .arg(distance).toStdString();
    View3dPtr()->renderText(x, y + 15, str.c_str(), mFont);
}
