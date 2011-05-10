#pragma once

#include "common/omCommon.h"

#include <QtGui>

class MainWindow;
class MainToolBar;
class DendToolBar;
class OmViewGroupState;

namespace om { namespace sidebars { class right; } }

class ToolBarManager : public QWidget {
Q_OBJECT

public:
    ToolBarManager(MainWindow* mw);
    ~ToolBarManager();

    void UpdateReadOnlyRelatedWidgets();
    void UpdateGuiFromProjectLoadOrOpen(OmViewGroupState *);
    void UpdateGuiFromProjectClose();

    void SetSplittingOff();
    void SetCuttingOff();

    void SetTool(const om::tool::mode tool);

private:
    MainWindow *const mainWindow_;

    boost::scoped_ptr<MainToolBar> mainToolBar_;
    boost::scoped_ptr<om::sidebars::right> rightSideBar_;

    void deleteMainBar();
};

