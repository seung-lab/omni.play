#pragma once

#include "view3d/omView3dWidget.h"
#include "common/omCommon.h"
#include "volume/omSegmentation.h"
#include <QFont>

struct annotation
{
    QString text;
    DataCoord point;
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
