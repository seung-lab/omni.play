#pragma once

#include "common/common.h"
#include "zi/omUtility.h"

#include <QSize>
#include <QUndoStack>
#include <QGLWidget>
#include <QGLContext>

class MainWindow;
class InspectorWidget;
class ToolBarManager;

namespace om { namespace sidebars { class rightImpl; } }

class OmAppState : private om::singletonBase<OmAppState> {
private:
    InspectorWidget* inspectorWidget_;
    MainWindow* mainWindow_;
    om::sidebars::rightImpl* rightToolbar_;
    ToolBarManager* toolBarManager_;

public:
    static QString GetPID();
    static QString GetHostname();

    static void SetInspector(InspectorWidget* miw);
    static void SetMainWindow(MainWindow* mw);

    static void SetRightToolBar(om::sidebars::rightImpl* rightToolbar){
        instance().rightToolbar_ = rightToolbar;
    }

    static ToolBarManager* GetToolBarManager(){
        return instance().toolBarManager_;
    }
    static void SetToolBarManager(ToolBarManager* tbm);

    static void UpdateStatusBar(const QString & msg);

    static QSize GetViewBoxSizeHint();

private:
    OmAppState()
        : inspectorWidget_(NULL)
        , mainWindow_(NULL)
        , rightToolbar_(NULL)
        , toolBarManager_(NULL)
    {}

    ~OmAppState(){};

    friend class zi::singleton<OmAppState>;
};

