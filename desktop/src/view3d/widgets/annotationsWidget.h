#pragma once

#include "view3d/omView3dWidget.h"
#include "common/common.h"
#include "volume/omSegmentation.h"
#include <QFont>

struct annotation
{
    QString text;
    om::globalCoord point;
    OmColor color;
};

class AnnotationsWidget : public OmView3dWidget {

    public:
        AnnotationsWidget(OmView3d *view3d, OmViewGroupState *vgs);
        virtual void Draw();

    private:
        QFont font_;
        static const float DIST_CUTOFF = 100.0f;
        OmViewGroupState *vgs_;
};
