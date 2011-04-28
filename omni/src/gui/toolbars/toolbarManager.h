#ifndef TOOLBAR_MANAGER_H
#define TOOLBAR_MANAGER_H

#include "common/omCommon.h"

#include <QtGui>

class MainWindow;
class MainToolBar;
class DendToolBar;
class OmViewGroupState;

class ToolBarManager : public QWidget {
Q_OBJECT

public:
    ToolBarManager(MainWindow* mw);
    ~ToolBarManager();

    void UpdateReadOnlyRelatedWidgets();
    void UpdateGuiFromProjectLoadOrOpen(OmViewGroupState *);
    void UpdateGuiFromProjectClose();
    void WindowResized(QPoint oldPos);
    void WindowMoved(QPoint oldPos);

    void SetSplittingOff();

    void SetCuttingOff();
    void SetTool(const om::tool::mode tool);

private:
    MainWindow *const mainWindow_;

    boost::scoped_ptr<MainToolBar> mainToolBar_;
    boost::scoped_ptr<DendToolBar> dendToolBar_;

    void deleteDendBar();
    void deleteMainBar();
};

#endif
