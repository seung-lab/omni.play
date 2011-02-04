#ifndef OM_STATE_MANAGER_IMPL_HPP
#define OM_STATE_MANAGER_IMPL_HPP

#include "common/omCommon.h"
#include "common/omDebug.h"
#include "system/omEvents.h"

#include <QtOpenGL/qgl.h>
#include <QtOpenGL/QGLFormat>

#include <QSize>
#include <QUndoStack>
#include <QGLWidget>

class OmStateManagerImpl {
private:
	boost::scoped_ptr<QUndoStack> undoStack_;

	OmToolMode toolModeCur_;
	OmToolMode toolModePrev_;

	//view3d context
	boost::scoped_ptr<QGLWidget> view3dWidget_;

public:
	OmStateManagerImpl()
		: undoStack_(new QUndoStack())
		, toolModeCur_(PAN_MODE)
		, toolModePrev_(PAN_MODE)
	{}

	~OmStateManagerImpl();

/////////////////////////////////
///////          Tool Mode

	inline OmToolMode GetToolMode() const {
		return toolModeCur_;
	}

	inline void SetToolModeAndSendEvent(const OmToolMode tool)
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

	inline QUndoStack* GetUndoStack(){
		return undoStack_.get();
	}

	inline void PushUndoCommand(QUndoCommand* cmd){
		undoStack_->push(cmd);
	}

	inline void ClearUndoStack(){
		undoStack_->clear();
	}

	inline void UndoUndoCommand(){
		undoStack_->undo();
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
