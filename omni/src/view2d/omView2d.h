#ifndef OM_VIEW2D_H
#define OM_VIEW2D_H

#include "view2d/omView2dState.hpp"
#include "view2d/omView2dCore.h"
#include "system/omStateManager.h"

#include <boost/scoped_ptr.hpp>

class OmView2dEvents;
class SegmentDataWrapper;
class OmMouseEvents;
class OmKeyEvents;
class OmView2dZoom;

class OmView2d : public OmView2dCore {
    Q_OBJECT

public:
    OmView2d(const ViewType, QWidget*, OmViewGroupState*, OmMipVolume*,
             const std::string& name);
    ~OmView2d();

    void SetComplimentaryDockWidget(QDockWidget* dock){
        complimentaryDock_ = dock;
    }

    void ShowComplimentaryDock()
    {
        if(complimentaryDock_)
        {
            complimentaryDock_->raise();
            complimentaryDock_->widget()->setFocus(Qt::OtherFocusReason);
        }
    }

    inline OmView2dZoom* Zoom(){
        return zoom_.get();
    }

    void ResetWidget();

    void Redraw();
    void RedrawBlocking();

protected:
    void keyPressEvent (QKeyEvent *event);

    // mouse events
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);
    void enterEvent(QEvent*);

    QSize sizeHint () const;

private:
    OmView2dState *const state_;
    QDockWidget* complimentaryDock_;

    boost::scoped_ptr<OmMouseEvents> mouseEvents_;
    boost::scoped_ptr<OmKeyEvents> keyEvents_;
    boost::scoped_ptr<OmView2dEvents> events_;
    boost::scoped_ptr<OmView2dZoom> zoom_;

    void unlinkComplimentaryDock();
};

#endif
