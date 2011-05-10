#pragma once

#include "common/omCommon.h"
#include "view2d/omOnScreenTileCoords.h"

#include <QGLWidget>
#include <QtGui>

class OmFilter2d;
class OmScreenPainter;
class OmTextureID;
class OmTileCache;
class OmTileDrawer;
class OmView2dState;
class OmViewGroupState;

class OmView2dCore : public QGLWidget {
Q_OBJECT

public:
    inline ViewType GetViewType() const {
        return viewType_;
    }

    bool IsDrawComplete();
    int GetTileCount();
    int GetTileCountIncomplete();

public Q_SLOTS:
    void dockVisibilityChanged(const bool visible);

protected:
    OmView2dCore(QWidget *, OmMipVolume*, OmViewGroupState*, const ViewType,
                 const std::string& name);

    virtual ~OmView2dCore();

    inline OmView2dState* State() {
        return state_.get();
    }

    // QT QGLWidget overrides
    void initializeGL();
    void resizeGL(int width, int height);
    virtual void paintGL();

    bool blockingRedraw_;

private:
    const ViewType viewType_;
    const std::string name_;

    boost::scoped_ptr<OmView2dState> state_;
    boost::scoped_ptr<OmTileDrawer> tileDrawer_;
    boost::scoped_ptr<OmScreenPainter> screenPainter_;

    void setupMainGLpaintOp();
    void teardownMainGLpaintOp();
};

