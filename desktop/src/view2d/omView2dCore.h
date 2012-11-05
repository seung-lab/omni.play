#pragma once

#include "common/common.h"
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
#include "view2d/details/omView2dWidgetMac.hpp"
#else
#include "view2d/details/omView2dWidgetLinux.hpp"
#endif

class OmView2dCore : public OmView2dWidgetBase {
Q_OBJECT

public:
    inline om::common::ViewType GetViewType() const {
        return viewType_;
    }

    bool IsDrawComplete();
    int GetTileCount();
    int GetTileCountIncomplete();

public Q_SLOTS:
    void dockVisibilityChanged(const bool visible);

protected:
    OmView2dCore(QWidget *, OmMipVolume*, OmViewGroupState*, const om::common::ViewType,
                 const std::string& name);

    virtual ~OmView2dCore();

    inline OmView2dState* State() {
        return state_.get();
    }

    void Initialize();
    void Paint3D();
    void PaintOther();
    void Resize(int width, int height);

    bool blockingRedraw_;

private:
    const om::common::ViewType viewType_;
    const std::string name_;

    boost::scoped_ptr<OmView2dState> state_;
    boost::scoped_ptr<OmTileDrawer> tileDrawer_;
    boost::scoped_ptr<OmScreenPainter> screenPainter_;


    void setupMainGLpaintOp();
    void teardownMainGLpaintOp();
};

