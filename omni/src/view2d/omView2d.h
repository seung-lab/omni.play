#ifndef OM_VIEW2D_H
#define OM_VIEW2D_H

#include "view2d/omView2dState.hpp"
#include "view2d/omView2dCore.h"
#include "system/omStateManager.h"
#include "view2d/omView2dKeyPressEventListener.h"

#include <boost/scoped_ptr.hpp>

class OmView2dEvents;
class OmScreenPainter;
class SegmentDataWrapper;
class OmScreenShotSaver;
class OmMouseEvents;
class OmKeyEvents;
class OmView2dZoom;

class OmView2d : public OmView2dCore, public OmView2dKeyPressEventListener {
Q_OBJECT

public:
	OmView2d(const ViewType, QWidget*, OmViewGroupState*, OmMipVolume*,
			 const std::string& name);
 	~OmView2d();

	void SetComplimentaryDockWidget(QDockWidget* dock){
		complimentaryDock_ = dock;
	}

	void ShowComplimentaryDock(){
		if(complimentaryDock_){
			complimentaryDock_->raise();
			complimentaryDock_->widget()->setFocus(Qt::OtherFocusReason);
		}
	}

	bool amInFillMode(){
		return OmStateManager::GetToolMode() == FILL_MODE;
	}

	inline OmScreenShotSaver* GetScreenShotSaver(){
		return mScreenShotSaver.get();
	}

	inline OmView2dZoom* Zoom(){
		return zoom_.get();
	}

	void resetWindow();
	void doRedraw2d();
	void SetDepth(QMouseEvent *event);

	void myUpdate();

protected:
	// GL event methods
	void resizeGL(const QSize&);
	void resizeEvent (QResizeEvent * event);

	void paintEvent(QPaintEvent *);

	void keyPressEvent (QKeyEvent *event);

	// mouse events
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent * event);

	QSize sizeHint () const;

private:
	QDockWidget* complimentaryDock_;

	boost::scoped_ptr<OmScreenShotSaver> mScreenShotSaver;
	boost::scoped_ptr<OmScreenPainter> screenPainter_;
	boost::scoped_ptr<OmMouseEvents> mouseEvents_;
	boost::scoped_ptr<OmKeyEvents> keyEvents_;
	boost::scoped_ptr<OmView2dEvents> events_;
	boost::scoped_ptr<OmView2dZoom> zoom_;

	void unlinkComplimentaryDock();
};

#endif
