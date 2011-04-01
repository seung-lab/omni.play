#ifndef OM_APP_STATE_HPP
#define OM_APP_STATE_HPP

#include "common/omCommon.h"
#include "zi/omUtility.h"

#include <QSize>
#include <QUndoStack>
#include <QGLWidget>
#include <QGLContext>

class DendToolBar;
class MainWindow;
class InspectorWidget;
class ToolBarManager;

class OmAppState : private om::singletonBase<OmAppState> {
private:
    InspectorWidget* inspectorWidget_;
    MainWindow* mainWindow_;
    DendToolBar* dendToolBar_;
    ToolBarManager* toolBarManager_;

public:
    static QString GetPID();
    static QString GetHostname();

    static void SetInspector(InspectorWidget* miw);
    static void SetMainWindow(MainWindow* mw);
    static void SetDendToolBar(DendToolBar* dtb);

    static DendToolBar* GetDendToolBar(){
        return instance().dendToolBar_;
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
        , dendToolBar_(NULL)
        , toolBarManager_(NULL)
    {}

    ~OmAppState(){};

    friend class zi::singleton<OmAppState>;
};

#endif
