#ifndef DEND_TOOLBAR_H
#define DEND_TOOLBAR_H

#include "common/omCommon.h"

#include <QtGui>

class MainWindow;
class OmViewGroupState;
class SegmentationDataWrapper;
class GraphTools;
class ValidationGroup;
class DisplayTools;
class OmWidget;

class DendToolBar : public QToolBar {
Q_OBJECT
public:
    DendToolBar( MainWindow* mw, OmViewGroupState* vgs );

    void updateToolBarsPos(QPoint oldPos);

    void updateGui();

    void SetSplittingOff();
    void SetCuttingOff();

    void RefreshThreshold();

    inline OmViewGroupState* getViewGroupState(){
        return vgs_;
    }

    SegmentationDataWrapper GetSDW();

    static bool GetShowGroupsMode();

private:
    MainWindow *const mainWindow_;
    OmViewGroupState* vgs_;

    GraphTools* graphTools;
    ValidationGroup* validationGroup;
    DisplayTools* displayTools;

    QDockWidget* graphToolsDock;
    QDockWidget* validationGroupDock;
    QDockWidget* displayToolsDock;

    QWidget* wrapWithGroupBox(OmWidget* widget);
    QDockWidget* makeDockWidget(OmWidget* widget);
    int moveToolDock(QDockWidget* toolDock, QPoint );
    QPoint recalcPos(QPoint oldPos, const int height);
};

#endif
