#ifndef MAIN_TOOLBAR_H
#define MAIN_TOOLBAR_H

#include "common/omCommon.h"

#include <QtGui>

class MainWindow;
class NavAndEditButtonGroup;
class OmViewGroupState;
class OpenViewGroupButton;
class OpenSingleViewButton;
class SaveButton;
class VolumeRefreshButton;
class BrushToolbox;

class MainToolBar : public QToolBar {
Q_OBJECT

public:
    MainToolBar(MainWindow* mw);

    void UpdateToolbar();
    void SetTool(const om::tool::mode tool);

private:
    MainWindow *const mainWindow_;

    SaveButton *const saveButton;
    OpenViewGroupButton *const openViewGroupButton_;
    OpenSingleViewButton *const openSingleViewButton_;
    VolumeRefreshButton *const volumeRefreshButton;
    NavAndEditButtonGroup *const navEditButtons_;

    BrushToolbox* brushToolbox_;

    void addNavEditButtons();
    void setReadOnlyWidgetsEnabled();
    void setModifyWidgetsEnabled();
};

#endif
