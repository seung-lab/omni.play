#pragma once

#include "common/omCommon.h"
#include "view2d/omOnScreenTileCoords.h"

#include <QWidget>
#include <QGLPixelBuffer>
#include <QtGui>

class OmFilter2d;
class OmScreenPainter;
class OmTextureID;
class OmTileCache;
class OmTileDrawer;
class OmView2dState;
class OmViewGroupState;

#ifdef ZI_OS_MACOS
class OmView2dCore : public QWidget {
#else
class OmView2dCore : public QGLWidget {
#endif

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

#ifdef ZI_OS_MACOS
    // QT QWidget overrides
	void paintEvent (QPaintEvent* event);
	void resizeEvent (QResizeEvent* event);
	void resetPbuffer(const QSize& size);
#else
    // QT QGLWidget overrides
    void initializeGL();
    void resizeGL(int width, int height);
    virtual void paintGL();
#endif

    void doPaintGL();
    void doPaintOther();
    void doResize(int width, int height);

    bool blockingRedraw_;

private:
    const ViewType viewType_;
    const std::string name_;

    boost::scoped_ptr<OmView2dState> state_;
    boost::scoped_ptr<OmTileDrawer> tileDrawer_;
    boost::scoped_ptr<OmScreenPainter> screenPainter_;

#ifdef ZI_OS_MACOS
    boost::scoped_ptr<QGLPixelBuffer> buffer_;
#endif

    void setupMainGLpaintOp();
    void teardownMainGLpaintOp();
};

