#ifndef DISPLAY_TOOLS_H
#define DISPLAY_TOOLS_H

#include <QtGui>

class DendToolBar;
class OmViewGroupState;
class SegmentationDataWrapper;

class DisplayTools : public QWidget {
Q_OBJECT
public:
    DisplayTools(DendToolBar*, OmViewGroupState* vgs);

    OmViewGroupState* GetViewGroupState(){
        return vgs_;
    }

    void updateGui();

    QString getName(){
        return "Display Tools";
    }

private:
    OmViewGroupState *const vgs_;

    QWidget* view2dSliceDepthBox();
    QWidget* thresholdBox();
};

#endif
