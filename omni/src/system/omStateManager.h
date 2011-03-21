#ifndef OM_STATE_MANAGER_H
#define OM_STATE_MANAGER_H

/*
 * Manages data structures that are shared between various parts of the system.
 *
 * Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/omCommon.h"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "zi/omUtility.h"

#include <QSize>
#include <QGLWidget>
#include <QGLContext>

class OmUndoStack;
class OmBrushSize;
class OmStateManagerImpl;

class OmStateManager : private om::singletonBase<OmStateManager> {
private:
    inline static OmStateManagerImpl& impl(){
        return OmProject::Globals().StateManagerImpl();
    }

public:
    static OmBrushSize* BrushSize();

    //tool mode
    static om::tool::mode GetToolMode();
    static void SetToolModeAndSendEvent(const om::tool::mode mode);
    static void SetOldToolModeAndSendEvent();

    //undostack
    static OmUndoStack& UndoStack();

    //view3d context
    static const QGLWidget* GetPrimaryView3dWidget();

private:
    OmStateManager();
    ~OmStateManager();

    friend class zi::singleton<OmStateManager>;
};

#endif
