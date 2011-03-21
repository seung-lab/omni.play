#ifndef OM_STATE_MANAGER_IMPL_HPP
#define OM_STATE_MANAGER_IMPL_HPP

#include "common/omCommon.h"
#include "common/omDebug.h"
#include "events/omEvents.h"
#include "viewGroup/omBrushSize.hpp"
#include "system/omUndoStack.hpp"

#include <QtOpenGL/qgl.h>
#include <QtOpenGL/QGLFormat>

#include <QSize>
#include <QGLWidget>

class OmStateManagerImpl {
private:
    om::tool::mode toolModeCur_;
    om::tool::mode toolModePrev_;

    OmBrushSize brushSize_;
    OmUndoStack undoStack_;

    //view3d context
    boost::scoped_ptr<QGLWidget> view3dWidget_;

public:
    OmStateManagerImpl()
        : toolModeCur_(om::tool::PAN)
        , toolModePrev_(om::tool::PAN)
    {}

    ~OmStateManagerImpl();

    OmBrushSize* BrushSize() {
        return &brushSize_;
    }

/////////////////////////////////
///////          Tool Mode

    inline om::tool::mode GetToolMode() const {
        return toolModeCur_;
    }

    inline void SetToolModeAndSendEvent(const om::tool::mode tool)
    {
        if (tool == toolModeCur_){
            return;
        }

        toolModePrev_ = toolModeCur_;
        toolModeCur_ = tool;

        OmEvents::ToolChange();
    }

    inline void SetOldToolModeAndSendEvent()
    {
        std::swap(toolModePrev_, toolModeCur_);

        OmEvents::ToolChange();
    }

/////////////////////////////////
///////          UndoStack

    inline OmUndoStack& UndoStack(){
        return undoStack_;
    }

/////////////////////////////////
///////          View3d Context

    const QGLWidget* GetPrimaryView3dWidget()
    {
        if(!view3dWidget_){
            createPrimaryView3dWidget();
        }

        return view3dWidget_.get();
    }

private:
    void createPrimaryView3dWidget()
    {
        view3dWidget_.reset(new QGLWidget());
        view3dWidget_->setFormat(QGLFormat(QGL::DoubleBuffer |
                                           QGL::DepthBuffer));
    }
};

#endif
