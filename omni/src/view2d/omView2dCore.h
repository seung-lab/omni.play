#ifndef OM_VIEW_2D_IMPL_H
#define OM_VIEW_2D_IMPL_H

#include "common/omCommon.h"
#include "view2d/omOnScreenTileCoords.h"

#include <QImage>
#include <QGLPixelBuffer>
#include <QtGui>

class OmFilter2d;
class OmLineDraw;
class OmTextureID;
class OmTileCache;
class OmTileDrawer;
class OmView2dState;
class OmViewGroupState;

class OmView2dCore : public QWidget {
Q_OBJECT
public:
	const ViewType mViewType;
	QImage fullRedraw();
	bool isDrawComplete();
	int getTileCount();
	int getTileCountIncomplete();

	const boost::shared_ptr<OmLineDraw>& getLineDrawer(){
		return lineDraw_;
	}

public slots:
	void dockVisibilityChanged(const bool visible);

protected:
	OmView2dCore(QWidget *, OmMipVolume*, OmViewGroupState*, const ViewType,
		     const std::string& name);

	void resetPbuffer(const QSize&);

	const std::string name_;

	boost::shared_ptr<OmView2dState> state_;


private:
	boost::shared_ptr<QGLPixelBuffer> pbuffer_;
	boost::shared_ptr<OmTileDrawer> tileDrawer_;
	boost::shared_ptr<OmLineDraw> lineDraw_;

	int mNearClip;
	int mFarClip;

	void reset();
	void setupMainGLpaintOp();
	void teardownMainGLpaintOp();

	friend class OmTileDrawer;
};

#endif
