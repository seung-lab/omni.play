#pragma once

#include "common/omCommon.h"

#include <QtGui>

class GraphTools;
class MainWindow;
class OmViewGroupState;
class SegmentationDataWrapper;

class DendToolBar : public QWidget {
Q_OBJECT
public:
    DendToolBar(MainWindow* mw, OmViewGroupState* vgs);

    void updateGui();

    void SetSplittingOff();
    void SetCuttingOff();

    void RefreshThreshold();

    SegmentationDataWrapper GetSDW();

    static bool GetShowGroupsMode();

private:
    MainWindow *const mainWindow_;
    OmViewGroupState *const vgs_;

    GraphTools* graphTools_;

    template <class T>
    QWidget* wrapWithGroupBox(T* widget)
    {
        QGroupBox* gbox = new QGroupBox(widget->getName(), this);
        QVBoxLayout* vbox = new QVBoxLayout(gbox);
        vbox->addWidget(widget);
        gbox->setContentsMargins(0,0,0,0);

        return gbox;
    }
};

