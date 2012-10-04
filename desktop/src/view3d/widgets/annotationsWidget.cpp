#include "common/omGl.h"
#include "system/omStateManager.h"
#include "view3d/omCamera.h"
#include "view3d/omView3d.h"
#include "view3d/widgets/annotationsWidget.h"
#include "annotation/annotation.h"
#include "viewGroup/omViewGroupState.h"

static const float TEXT_COLOR[4] = { 1, 1, 1, 0.8 };

AnnotationsWidget::AnnotationsWidget(OmView3d *view3d, OmViewGroupState *vgs)
    : OmView3dWidget(view3d)
    , vgs_(vgs)
{
    //set font properties
    font_ = QFont("Helvetica", 12);
};

/////////////////////////////////
///////          Draw Methods

void AnnotationsWidget::Draw()
{
    if(!vgs_->getAnnotationVisible()) {
        return;
    }

    Vector3f camPos = View3dPtr()->GetCamera().GetPosition();

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    GLUquadricObj* quad = gluNewQuadric();

    FOR_EACH(i, SegmentationDataWrapper::ValidIDs())
    {
        SegmentationDataWrapper sdw(*i);

        om::annotation::manager &annotations = *sdw.GetSegmentation().Annotations();

        FOR_EACH(it, annotations)
        {
        	if(!it->second.Enabled) {
        		continue;
        	}
        	om::annotation::data& a = *it->second.Object;
        	om::globalCoord coord = a.coord.toGlobalCoord();
            float dist = camPos.distance(coord);

            glPushMatrix();
            glEnable(GL_LIGHTING | GL_DEPTH_TEST);
            glColor3ub(a.color.red, a.color.green, a.color.blue);
            glTranslatef(coord.x, coord.y, coord.z);
            gluSphere(quad, a.size * .005 * dist, 26, 13);

            glDisable(GL_LIGHTING | GL_DEPTH_TEST);
            glColor4fv(TEXT_COLOR);
            View3dPtr()->renderText(.4, -.2, 0, QString::fromStdString(a.comment), font_);
            glPopMatrix();
        }

    }

    gluDeleteQuadric(quad);

    glPopAttrib();
}
