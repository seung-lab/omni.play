#ifndef OM_VIEW_2D_IMPL_H
#define OM_VIEW_2D_IMPL_H

#include "common/omCommon.h"
#include "view2d/omOnScreenTileCoords.h"

#include <QImage>
#include <QGLPixelBuffer>
#include <QtGui>

class OmFilter2d;
class OmTextureID;
class OmTileCache;
class OmTileDrawer;
class OmView2dState;
class OmViewGroupState;

class OmView2dCore : public QWidget {
    Q_OBJECT
    public:
    inline ViewType GetViewType() const {
        return viewType_;
    }

    QImage FullRedraw2d();
    bool IsDrawComplete();
    int GetTileCount();
    int GetTileCountIncomplete();

public Q_SLOTS:
    void dockVisibilityChanged(const bool visible);

protected:
    OmView2dCore(QWidget *, OmMipVolume*, OmViewGroupState*, const ViewType,
                 const std::string& name);

    virtual ~OmView2dCore();

    void resetPbuffer(const QSize&);

    inline OmView2dState* State() {
        return state_.get();
    }

private:
    const ViewType viewType_;
    const std::string name_;

    boost::shared_ptr<OmView2dState> state_;
    boost::scoped_ptr<QGLPixelBuffer> pbuffer_;
    boost::scoped_ptr<OmTileDrawer> tileDrawer_;

    int nearClip_;
    int farClip_;

    void reset();
    void setupMainGLpaintOp();
    void teardownMainGLpaintOp();
};

#endif
