
#include "omView2d.h"

#include "omCachingTile.h"
#include "omTextureID.h"

#include "system/omStateManager.h"
#include "system/omProject.h"
#include "system/omSystemTypes.h"
#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "system/omGarbage.h"

#include "segment/omSegment.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#include "segment/actions/segment/omSegmentSelectionAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/voxel/omVoxelSetValueAction.h"
#include "segment/actions/segment/omSegmentMergeAction.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include "segment/omSegmentEditor.h"

#include "common/omGl.h"

#include <QPainter>
#include <QGLFramebufferObject>

#define DEBUG 0


static QGLWidget *sharedwidget = NULL;

/*
 *	Constructs View2d widget.
 */
OmView2d::OmView2d(ViewType viewtype, ObjectType voltype, OmId image_id, OmId second_id, OmId third_id,
		   QWidget * parent)
 : QWidget(parent)
{
	sharedwidget = (QGLWidget *) OmStateManager::GetPrimaryView3dWidget();
	//debug("genone","OmView2d::OmView2d -- " << viewtype);

	mViewType = viewtype;
	mVolumeType = voltype;
	mImageId = image_id;
	mSecondId = second_id;
	mThirdId = third_id;
	mAlpha = 1;
	mJoiningSegmentToggled = false;
	mLevelLock = false;
	mBrushToolDiameter = 1;
	mCurrentSegmentId = 0;
	mCurrentSegmentation = 0;
	mEditedSegmentation = 0;
	mElapsed = NULL;
	mBrushToolMaxX = 0;
	mBrushToolMaxY = 0;
	mBrushToolMaxZ = 0;
	mBrushToolMinX = INT_MAX;
	mBrushToolMinY = INT_MAX;
	mBrushToolMinZ = INT_MAX;
	mEmitMovie = false;
	mSlide = 0;
	mDoRefresh = false;
	mDragX = 0;
	mDragY = 0;
	mMIP = false;
	mScribbling = false;
	mAnimation = NULL;

	setFocusPolicy(Qt::ClickFocus);	// necessary for receiving keyboard events
	setMouseTracking(true);	// necessary for mouse-centered zooming
	setAutoFillBackground(false);	// necessary for proper QPainter functionality

	//setup widgets
	// mView3dWidgetManager.Set(1, new OmSelectionWidget(this));
	// mView3dWidgetManager.Set(2, new OmViewBoxWidget(this));
	// mView3dWidgetManager.Set(3, new OmInfoWidget(this));
	// mView3dWidgetManager.Set(4, new OmChunkExtentWidget(this));

	pbuffer = new QGLPixelBuffer(size(), QGLFormat::defaultFormat(), sharedwidget);
	mNewDraw = false;

	// set the cache up
	assert((mVolumeType == CHANNEL) || (mVolumeType == SEGMENTATION));

	if (mVolumeType == CHANNEL) {
		OmChannel & current_channel = OmVolume::GetChannel(mImageId);
		mVolume = &current_channel;

		//              mCache = new OmCachingTile(mViewType, mVolumeType, image_id, &current_channel);
		OmCachingThreadedCachingTile *fastCache =
		    new OmCachingThreadedCachingTile(mViewType, mVolumeType, image_id, &current_channel, NULL);
		mCache = fastCache->mCache;
		if (fastCache->mDelete)
			delete fastCache;

		mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);

		mCache->SetContinuousUpdate(false);

		if (mSecondId && mThirdId) {
			OmChannel & second_channel = OmVolume::GetChannel(mSecondId);
			OmChannel & third_channel = OmVolume::GetChannel(mThirdId);
			//debug("genone","THERE ARE THREE IDS");
			mCache->setSecondMipVolume(CHANNEL, mSecondId, &second_channel);
		}

		mRootLevel = current_channel.GetRootMipLevel();
	} else {
		OmSegmentation & current_seg = OmVolume::GetSegmentation(mImageId);
		mVolume = &current_seg;
		mSeg = &current_seg;

		//              mCache = new OmCachingTile(mViewType, mVolumeType, image_id, &current_seg);
		OmCachingThreadedCachingTile *fastCache =
		    new OmCachingThreadedCachingTile(mViewType, mVolumeType, image_id, &current_seg, NULL);
		mCache = fastCache->mCache;
		if (fastCache->mDelete)
			delete fastCache;

		mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);
		mCache->SetContinuousUpdate(false);

		if (mSecondId) {
			OmChannel & second_channel = OmVolume::GetChannel(mSecondId);
			mCache->setSecondMipVolume(CHANNEL, mSecondId, &second_channel);
		}

		mRootLevel = current_seg.GetRootMipLevel();
	}

	//debug("FIXME", << "conversion = " << endl;
	DataCoord test = DataCoord(0, 0, 0);
	SpaceCoord test2 = DataToSpaceCoord(test);
	//debug("FIXME", << "0 ---> " << test2.z << endl;

	test = DataCoord(0, 0, 1);
	test2 = DataToSpaceCoord(test);
	//debug("FIXME", << "1 ---> " << test2.z << endl;

	cameraMoving = false;

	mSegmentID = 0;
	mSegmentValue = 0;

	sentTexture = false;

	depthCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT);
	sidesCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT);
	mipCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT);

	delete_dirty = false;

	iSentIt = false;

	OmCachingThreadedCachingTile::Refresh();
}

OmView2d::~OmView2d()
{
	OmCachingThreadedCachingTile::Refresh();
}

#pragma mark
#pragma mark GL Event Methods
/////////////////////////////////
///////          GL Event Methods

// The initializeGL() function is called just once, before paintGL() is called.
// More importantly this function is called before "make current" calls.
void OmView2d::initializeGL()
{
	// IMPORTANT: To cooperate fully with QPainter, we defer matrix stack operations and attribute initialization until
	// the widget needs to be myUpdated.
	//debug("genone","OmView2d::initializeGL        " << "(" << size().width() << ", " << size().height() << ")");
	//debug("genone","viewtype = " << mViewType);

	mTotalViewport.lowerLeftX = 0;
	mTotalViewport.lowerLeftY = 0;
	mTotalViewport.width = size().width();
	mTotalViewport.height = size().height();

	// //debug("FIXME", << "mtotalviewport = " << mTotalViewport << endl;

	mNearClip = -1;
	mFarClip = 1;
	mZoomLevel = 0;

	// //debug("FIXME", << "mTotalViewport = " << mTotalViewport << endl;
}

void OmView2d::resizeEvent(QResizeEvent * event)
{
	resizeGL(event->size().width(), event->size().height());
	myUpdate();
}

/*
 *	Window resize event
 */
void OmView2d::resizeGL(int width, int height)
{
	//debug("genone","OmView2d::resizeGL(" << width << ", " << height << ")");

	OmCachingThreadedCachingTile::Refresh();

	delete pbuffer;
	pbuffer = new QGLPixelBuffer(QSize(width, height), QGLFormat::defaultFormat(), sharedwidget);


	mTotalViewport.lowerLeftX = 0;
	mTotalViewport.lowerLeftY = 0;
	mTotalViewport.width = width;
	mTotalViewport.height = height;

	SetViewSliceOnPan();
}

/*
 *	Paint window event.
 */
void OmView2d::paintEvent(QPaintEvent * event)
{
	boost::timer t;
	float zoomFactor = OmStateManager::Instance()->GetZoomLevel().y / 10.0;

	mImage = safePaintEvent(event);

	QPainter painter;
	painter.begin(this);
	painter.drawImage(QPoint(0, 0), mImage);

	if (mEmitMovie) {
		QString file = QString("omniss-%1.png").arg(mSlide);
		if (!mImage.save(file, "png")) ;
		//debug("FIXME", << "failed to make slide " << mSlide << ": " << file.toStdString() << endl;
		mSlide++;
	}

	QPen the_pen;
	switch (mViewType) {
	case XY_VIEW:
		the_pen.setColor(QColor(Qt::blue));
		break;
	case XZ_VIEW:
		the_pen.setColor(QColor(Qt::green));
		break;
	case YZ_VIEW:
		the_pen.setColor(QColor(Qt::red));
		break;
	}
	painter.setPen(the_pen);

	if (amInFillMode()) {
		painter.drawRoundedRect(QRect(mMousePoint.x, mMousePoint.y, 20, 20), 5, 5);
	} else if (OmStateManager::GetSystemMode() == EDIT_SYSTEM_MODE) {
		painter.
		    drawEllipse(QRectF
				(mMousePoint.x - 0.5 * mBrushToolDiameter * zoomFactor,
				 mMousePoint.y - 0.5 * mBrushToolDiameter * zoomFactor,
				 1.0 * mBrushToolDiameter * zoomFactor, 1.0 * mBrushToolDiameter * zoomFactor));
	}

	if (hasFocus())
		the_pen.setWidth(5);
	painter.drawRect(mTotalViewport.lowerLeftX, mTotalViewport.lowerLeftY, mTotalViewport.width - 1,
			 mTotalViewport.height - 1);

	if ((!cameraMoving) && drawComplete && (!sentTexture)) {
		SendFrameBuffer(&mImage);
	} else if (!drawComplete)
		sentTexture = false;

	if (doDisplayInformation()) {
		QString str = QString("MIP Level Locked (Press L to unlock.)");
		if (mLevelLock)
			painter.drawText(QPoint(0, mTotalViewport.height - 80), str);

		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
		str = QString::number(zoomMipVector.x, 'f', 2) + QString(" Level");
		painter.drawText(QPoint(0, mTotalViewport.height - 80), str);

		float zoomFactor = (zoomMipVector.y / 10.0);
		str = QString::number(zoomFactor, 'f', 2) + QString(" zoomFactor");
		painter.drawText(QPoint(0, mTotalViewport.height - 60), str);

		//		float sliceDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
		//		str = QString::number(sliceDepth, 'f', 2) + QString(" Slice Depth");
		//		painter.drawText(QPoint(0, mTotalViewport.height - 40), str);

		float tiles = 10;
		str = QString::number(mTileCount, 'f', 0) + QString(" tile(s)");
		painter.drawText(QPoint(0, mTotalViewport.height - 20), str);

		if (!mScribbling) {
			str = QString::number(t.elapsed(), 'f', 4) + QString(" s");
			painter.drawText(QPoint(0, mTotalViewport.height), str);
		} else {
			if (mElapsed) {
				str = QString::number(1.0 / mElapsed->elapsed(), 'f', 4) + QString(" fps");
				painter.drawText(QPoint(0, mTotalViewport.height), str);
				mElapsed = new boost::timer();
			} else {
				mElapsed = new boost::timer();
			}
		}
	}

	painter.end();

	SetViewSliceOnPan();
}

QImage OmView2d::safePaintEvent(QPaintEvent * event)
{
	//debug("genone","OmView2d::paintEvent -- " << mViewType);
	//      //debug("genone","mTotalViewport = " << mTotalViewport);

	mTextures.clear();

	mTileCount = 0;
	initializeGL();
	pbuffer->makeCurrent();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix(); {

		glMatrixMode(GL_PROJECTION);
		glViewport(mTotalViewport.lowerLeftX, mTotalViewport.lowerLeftY, mTotalViewport.width,
			   mTotalViewport.height);
		glLoadIdentity();

		glOrtho(mTotalViewport.lowerLeftX,	/* left */
			mTotalViewport.width,	/* right */
			mTotalViewport.height,	/* bottom */
			mTotalViewport.lowerLeftY,	/* top */
			mNearClip, mFarClip);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_TEXTURE_2D);
		OmGarbage::safeCleanTextureIds();

		if (CHANNEL == mVolumeType) {
			float alpha = 0;

			glDisable(GL_BLEND);
			DrawFromCache();

			OmChannel & current_channel = OmVolume::GetChannel(mImageId);
			const set < OmId > objectIDs = current_channel.GetValidFilterIds();
			set < OmId >::iterator obj_it;

        		for( obj_it=objectIDs.begin(); obj_it != objectIDs.end(); obj_it++ ) {
        			OmFilter2d &filter = current_channel.GetFilter(*obj_it);

				alpha = filter.GetAlpha();
				glEnable(GL_BLEND);	// enable blending for transparency
				glBlendFunc(GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);
				glBlendColor(1.f, 1.f, 1.f, (1.f - alpha));

				//debug("FIXME", << filter.GetName() << " in alpha: " << alpha << " " << alpha*255 << endl;
				DrawFromFilter(filter);
				glDisable(GL_BLEND);	// enable blending for transparency
			}

		} else
			DrawFromCache();
		DrawCursors();
	}
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	pbuffer->doneCurrent();
	return pbuffer->toImage();
}

#pragma mark
#pragma mark MouseEvent Methods
/////////////////////////////////
///////          MouseEvent Methods

void OmView2d::Refresh()
{
	mDoRefresh = true;
}

void OmView2d::PickToolGetColor(QMouseEvent * event)
{
	QRgb p;
	int r, g, b, a;

	p = mImage.pixel(event->pos());
	r = qRed(mImage.pixel(event->pos()));
	g = qGreen(mImage.pixel(event->pos()));
	b = qBlue(mImage.pixel(event->pos()));
	a = qAlpha(mImage.pixel(event->pos()));

	//debug("FIXME",  << "p,r,g,b: " << p << ", " << r << ", " << g << ", " << b << endl;
	// FIXME: Need to add to hud.
}

void OmView2d::PickToolAddToSelection(OmId segmentation_id, DataCoord globalDataClickPoint)
{
	OmSegmentation & current_seg = OmVolume::GetSegmentation(segmentation_id);
	int theId = current_seg.GetVoxelSegmentId(globalDataClickPoint);
	if (theId && !current_seg.IsSegmentSelected(theId)) {
		(new OmSegmentSelectAction(segmentation_id, theId, true))->Run();
		Refresh();
	} else {
	}

}

DataCoord OmView2d::BrushToolOTGDC(DataCoord off)
{
	DataCoord ret;

	switch (mViewType) {
	case XY_VIEW:{
			ret.x = off.x;
			ret.y = off.y;
			ret.z = off.z;
		}
		break;
	case XZ_VIEW:{
			ret.x = off.x;
			ret.y = off.z;
			ret.z = off.y;
		}
		break;
	case YZ_VIEW:{
			ret.x = off.z;
			ret.y = off.y;
			ret.z = off.x;
		}
		break;
	}
	return ret;
}

static bool BrushTool8[8][8] = {
	{0, 0, 0, 1, 1, 0, 0, 0},

	{0, 0, 1, 1, 1, 1, 0, 0},

	{0, 1, 1, 1, 1, 1, 1, 0},

	{1, 1, 1, 1, 1, 1, 1, 1},

	{1, 1, 1, 1, 1, 1, 1, 1},

	{0, 1, 1, 1, 1, 1, 1, 0},

	{0, 0, 1, 1, 1, 1, 0, 0},

	{0, 0, 0, 1, 1, 0, 0, 0}
};

static bool BrushTool32[32][32] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},

	{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
	{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
	{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},

	{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
	{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},

	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
	{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
	{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
	{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},

	{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
	{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
	{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
	{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},

	{0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

void OmView2d::BrushToolApplyPaint(OmId segid, DataCoord gDC, SEGMENT_DATA_TYPE seg)
{
	DataCoord off;

	switch (mViewType) {
	case XY_VIEW:{
			off.x = gDC.x;
			off.y = gDC.y;
			off.z = gDC.z;
		}
		break;
	case XZ_VIEW:{
			off.x = gDC.x;
			off.y = gDC.z;
			off.z = gDC.y;
		}
		break;
	case YZ_VIEW:{
			off.x = gDC.z;
			off.y = gDC.y;
			off.z = gDC.x;
		}
		break;
	}

	if (1 == mBrushToolDiameter) {
		//(new OmVoxelSetValueAction(segid, gDC, seg))->Run();
		if (segid != 1 && segid != 0) {
			//debug("FIXME", << segid << " is the seg id" << endl;
		}
		mEditedSegmentation = segid;
		mCurrentSegmentId = seg;
		mUpdateCoordsSet.insert(gDC);
		if (gDC.x > mBrushToolMaxX) {
			mBrushToolMaxX = gDC.x;
		}
		if (gDC.y > mBrushToolMaxY) {
			mBrushToolMaxY = gDC.y;
		}
		if (gDC.z > mBrushToolMaxZ) {
			mBrushToolMaxZ = gDC.z;
		}

		if (gDC.x < mBrushToolMinX) {
			mBrushToolMinX = gDC.x;
		}
		if (gDC.y < mBrushToolMinY) {
			mBrushToolMinY = gDC.y;
		}
		if (gDC.z < mBrushToolMinZ) {
			mBrushToolMinZ = gDC.z;
		}

	} else if (32 == mBrushToolDiameter && 0) {
		int savedDia = mBrushToolDiameter;
		mBrushToolDiameter = 1;
		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 32; j++) {
				DataCoord myoff = off;
				if (BrushTool32[i][j]) {
					myoff.x += i - 16;
					myoff.y += j - 16;
					BrushToolApplyPaint(segid, BrushToolOTGDC(myoff), seg);
				}
			}
		}
		mBrushToolDiameter = savedDia;
	} else {
		int savedDia = mBrushToolDiameter;
		mBrushToolDiameter = 1;
		for (int i = 0; i < savedDia; i++) {
			for (int j = 0; j < savedDia; j++) {
				DataCoord myoff = off;
				float x = i - savedDia / 2.0;
				float y = j - savedDia / 2.0;
				if (sqrt(x * x + y * y) <= savedDia / 2.0) {
					myoff.x += i - savedDia / 2.0;
					myoff.y += j - savedDia / 2.0;
					BrushToolApplyPaint(segid, BrushToolOTGDC(myoff), seg);
				}
			}
		}
		mBrushToolDiameter = savedDia;
	}
}

void OmView2d::mouseDoubleClickEvent(QMouseEvent * event)
{
	switch (OmStateManager::GetSystemMode()) {
	case NAVIGATION_SYSTEM_MODE:
		break;
	case EDIT_SYSTEM_MODE:
		break;
	}
}

// FIXME: what is going on here? why does it work??
void OmView2d::SetDepth(QMouseEvent * event)
{
	Vector2f clickPoint = Vector2f(event->x(), event->y());

	int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
	int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;

	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	float scaleFactor = (zoomMipVector.y / 10.0);

	Vector2f localClickPoint =
	    Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);

	float newDepthX = DataToSpaceCoord(SpaceCoord(0, 0, localClickPoint.x)).z;
	float newDepthY = DataToSpaceCoord(SpaceCoord(0, 0, localClickPoint.y)).z;

	debug ("view2d", "click event x,y (%f, %f)\n", clickPoint.x, clickPoint.y);
	debug ("view2d", "newDepth x,y,z (%f, %f,)\n", newDepthX, newDepthY);
	debug ("view2d", "scaleFactor*newDepth x,y (%f, %f, %f)\n", newDepthX*scaleFactor, newDepthY*scaleFactor);
	switch (mViewType) {
	case XY_VIEW:{
			OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, newDepthX);
			OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, newDepthY);
		}
		break;
	case XZ_VIEW:{
			OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, newDepthX);
			OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, newDepthY);
		}
		break;
	case YZ_VIEW:{
			OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, newDepthX);
			OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, newDepthY);
		}
		break;
	}

	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
}

// XY_VIEW is the default viewType 
// (different newTypes only used by mouseMove_NavMode_DrawInfo(...) for some reason??? (purcaro)
DataCoord OmView2d::getMouseClickpointLocalDataCoord(QMouseEvent * event, const ViewType viewType)
{
	Vector2f clickPoint = Vector2f(event->x(), event->y());

	int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
	int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
	float depth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);

	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
        float pl = pow(2, zoomMipVector.x);
	float scaleFactor = (zoomMipVector.y / 10.0);

	Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor*pl) - widthTranslate*pl,
					    (clickPoint.y / scaleFactor*pl) - heightTranslate*pl);

	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, depth));
	const int viewDepth = data_coord.z;

	DataCoord dataClickPoint;
	switch (viewType) {
	case XY_VIEW:
		dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, viewDepth);
		break;
	case XZ_VIEW:
		dataClickPoint = DataCoord(localClickPoint.x, viewDepth, localClickPoint.y);
		break;
	case YZ_VIEW:
		dataClickPoint = DataCoord(viewDepth, localClickPoint.y, localClickPoint.x);
		break;
	}

	return dataClickPoint;
}

DataCoord OmView2d::getMouseClickpointGlobalDataCoord(QMouseEvent * event)
{
	DataCoord dataClickPoint = getMouseClickpointLocalDataCoord(event);

	// okay, dataClickPoint is flat, only valid in XY ortho view.  
	//  This is not correct.  Needs to be global voxel coordinate.
	DataCoord globalDataClickPoint;
	switch (mViewType) {
	case XY_VIEW:
		globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
		break;
	case XZ_VIEW:
		globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
		break;
	case YZ_VIEW:
		globalDataClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
		break;
	}

	return globalDataClickPoint;
}

void OmView2d::mouseSelectSegment(QMouseEvent * event)
{
	//debug ("genone", "in mouseSelectSegment");

	bool augment_selection = event->modifiers() & Qt::ShiftModifier;

	int pick_object_type;

	// find segment selected
	DataCoord dataClickPoint = getMouseClickpointGlobalDataCoord(event);

	OmId theId;
	OmId segmentationID;
	bool found = false;
	if (mVolumeType == SEGMENTATION) {
		found = true;
		OmSegmentation & current_seg = OmVolume::GetSegmentation(mImageId);
		segmentationID = mImageId;
		theId = current_seg.GetVoxelSegmentId(dataClickPoint);
	} else {
		OmChannel & current_channel = OmVolume::GetChannel(mImageId);
		const set < OmId > objectIDs = current_channel.GetValidFilterIds();
		set < OmId >::iterator obj_it;

		for( obj_it=objectIDs.begin(); obj_it != objectIDs.end(); obj_it++ ) {
			OmFilter2d &filter = current_channel.GetFilter(*obj_it);
			if (filter.GetSegmentation ()) {
				found = true;
				segmentationID = filter.GetSegmentation();
				OmSegmentation & current_seg = OmVolume::GetSegmentation(segmentationID);
				theId = current_seg.GetVoxelSegmentId(dataClickPoint);
				break;
			}
		}
	}
	if (!found) {
		return;
	}

	OmSegmentation & segmentation = OmVolume::GetSegmentation(segmentationID);

	if (segmentation.IsSegmentValid(theId)) {

		OmSegmentEditor::SetEditSelection(segmentationID, theId);

		const bool curSegmentNotYetMarkedAsSelected = !(segmentation.IsSegmentSelected(theId));

		// if not augmenting slection and selecting segment, then 
		//  select new segment, and deselect current segment(s)
		if (!augment_selection && curSegmentNotYetMarkedAsSelected) {
			OmIds select_segment_ids;
			select_segment_ids.insert(theId);
			(new OmSegmentSelectAction(segmentationID,
						   select_segment_ids,
						   segmentation.GetSelectedSegmentIds(), theId))->Run();
		} else {
			(new OmSegmentSelectAction(segmentationID,
						   theId, curSegmentNotYetMarkedAsSelected, theId))->Run();
		}
		Refresh();
		mTextures.clear();
		myUpdate();
	}
}

#pragma mark
#pragma mark Edit Mode Methods
/////////////////////////////////
///////          Edit Mode Methods

void OmView2d::FillToolFill(OmId seg, DataCoord gCP, SEGMENT_DATA_TYPE fc, SEGMENT_DATA_TYPE bc, int depth)
{

	DataCoord off;
	OmId segid = OmVolume::GetSegmentation(seg).GetVoxelSegmentId(gCP);

	if (!segid)
		return;
	if (depth > 5000)
		return;
	depth++;

	//debug("FIXME", << gCP << " filling... in " << segid << " with fc of " << fc << "  and bc of " << bc <<  " at " << depth << endl;

	if (segid != fc && segid == bc) {

		switch (mViewType) {
		case XY_VIEW:{
				off.x = gCP.x;
				off.y = gCP.y;
				off.z = gCP.z;
			}
			break;
		case XZ_VIEW:{
				off.x = gCP.x;
				off.y = gCP.z;
				off.z = gCP.y;
			}
			break;
		case YZ_VIEW:{
				off.x = gCP.z;
				off.y = gCP.y;
				off.z = gCP.x;
			}
			break;
		}

		(new OmVoxelSetValueAction(seg, gCP, fc))->Run();
		//delete new OmVoxelSetValueAction(seg, gCP, fc);

		off.x++;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
		off.y++;
		off.x--;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
		off.x--;
		off.y--;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
		off.y--;
		off.x++;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
	}
}

void OmView2d::EditMode_MouseRelease_LeftButton_Filling(QMouseEvent * event)
{

	mScribbling = false;
	DataCoord dataClickPoint = getMouseClickpointLocalDataCoord(event);
	DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);

	//store current selection
	OmId segmentation_id, segment_id;
	bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);

	//return if not valid
	if (!valid_edit_selection)
		return;

	//switch on tool mode
	SEGMENT_DATA_TYPE data_value;
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//get value associated to segment id
		data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
		break;

	case SUBTRACT_VOXEL_MODE:
		data_value = NULL_SEGMENT_DATA;
		break;

	default:
		return;
		break;
	}

	OmId segid = OmVolume::GetSegmentation(segmentation_id).GetVoxelSegmentId(globalDataClickPoint);
	FillToolFill(segmentation_id, globalDataClickPoint, data_value, segid);
	Refresh();
	mTextures.clear();
	myUpdate();
}

void OmView2d::EditModeMouseRelease(QMouseEvent * event)
{
	// END PAINTING

	bool doselection = false;

	if (event->button() == Qt::RightButton) {
		return;
	}

	if (event->button() == Qt::LeftButton) {
		if (amInFillMode()) {
			EditMode_MouseRelease_LeftButton_Filling(event);
		} else if (mScribbling) {

			mScribbling = false;
			DataCoord dataClickPoint = getMouseClickpointLocalDataCoord(event);
			DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);

			//store current selection
			OmId segmentation_id, segment_id;
			bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);

			//return if not valid
			if (!valid_edit_selection)
				return;

			//switch on tool mode
			SEGMENT_DATA_TYPE data_value;
			switch (OmStateManager::GetToolMode()) {
			case ADD_VOXEL_MODE:
				//get value associated to segment id
				data_value =
				    OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
				break;

			case SUBTRACT_VOXEL_MODE:
				data_value = NULL_SEGMENT_DATA;
				break;

			case SELECT_VOXEL_MODE:
				doselection = true;
				break;

			default:
				return;
				break;
			}

			//run action
			if (!doselection)
				BrushToolApplyPaint(segmentation_id, globalDataClickPoint, data_value);
			else
				PickToolAddToSelection(segmentation_id, globalDataClickPoint);

			lastDataPoint = dataClickPoint;

			myUpdate();
		}
	}
}

void OmView2d::EditModeMouseMove(QMouseEvent * event)
{
	// KEEP PAINTING
	//debug ("genone", "scribbling? %i!\n", mScribbling);

	
	if (PAN_MODE == OmStateManager::GetToolMode()) {
		mouseMove_NavMode_CamMoving(event);
	} else if (mScribbling) {
		EditMode_MouseMove_LeftButton_Scribbling(event);
		return;
	}
}

void OmView2d::bresenhamLineDraw(const DataCoord & first, const DataCoord & second)
{
	//store current selection
	OmId segmentation_id, segment_id;
	bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
	bool doselection = false;

	//return if not valid
	if (!valid_edit_selection)
		return;

	//switch on tool mode
	SEGMENT_DATA_TYPE data_value;
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//get value associated to segment id
		data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
		break;

	case SUBTRACT_VOXEL_MODE:
		data_value = NULL_SEGMENT_DATA;
		break;

	case SELECT_VOXEL_MODE:
		doselection = true;
		break;

	default:
		//assert(false);
		break;
	}

	//debug("FIXME", << "first data coord = " << first << endl;
	//debug("FIXME", << "second data coord = " << second << endl;
	//debug("FIXME", << endl;

	float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
	int mViewDepth = data_coord.z;

	int y1 = second.y;
	int y0 = first.y;
	int x1 = second.x;
	int x0 = first.x;

	int dy = y1 - y0;
	int dx = x1 - x0;
	int stepx, stepy;

	if (dy < 0) {
		dy = -dy;
		stepy = -1;
	} else {
		stepy = 1;
	}
	if (dx < 0) {
		dx = -dx;
		stepx = -1;
	} else {
		stepx = 1;
	}
	dy <<= 1;		// dy is now 2*dy
	dx <<= 1;		// dx is now 2*dx

	// modifiedCoords.insert(DataCoord(x0, y0, mViewDepth));
	DataCoord myDC = DataCoord(x0, y0, mViewDepth);

	// myDC is flat, only valid for XY view.  This is not correct.

	DataCoord globalDC;
	switch (mViewType) {
	case XY_VIEW:{
			globalDC = DataCoord(myDC.x, myDC.y, myDC.z);
			//                                      //debug("FIXME", << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
		}
		break;
	case XZ_VIEW:{
			globalDC = DataCoord(myDC.x, myDC.z, myDC.y);
		}
		break;
	case YZ_VIEW:{
			globalDC = DataCoord(myDC.z, myDC.y, myDC.x);
		}
		break;
	}
	//debug("FIXME", << "global click point: " << globalDC << endl;

	if (!doselection)
		BrushToolApplyPaint(segmentation_id, globalDC, data_value);
	else
		PickToolAddToSelection(segmentation_id, globalDC);

	// //debug("FIXME", << "insert: " << DataCoord(x0, y0, 0) << endl;

	int lastx = x0 - mBrushToolDiameter / 4;
	int lasty = y0 - mBrushToolDiameter / 4;
	if (dx > dy) {
		int fraction = dy - (dx >> 1);	// same as 2*dy - dx
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;	// same as fraction -= 2*dx
			}
			x0 += stepx;
			fraction += dy;	// same as fraction -= 2*dy
			//modifiedCoords.insert(DataCoord(x0, y0, mViewDepth));
			DataCoord myDC = DataCoord(x0, y0, mViewDepth);

			// myDC is flat, only valid for XY view.  This is not correct.

			DataCoord globalDC;
			switch (mViewType) {
			case XY_VIEW:{
					globalDC = DataCoord(myDC.x, myDC.y, myDC.z);
					//                                      //debug("FIXME", << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
				}
				break;
			case XZ_VIEW:{
					globalDC = DataCoord(myDC.x, myDC.z, myDC.y);
				}
				break;
			case YZ_VIEW:{
					globalDC = DataCoord(myDC.z, myDC.y, myDC.x);
				}
				break;
			}
			//debug("FIXME", << "global click point: " << globalDC << endl;

			if (mBrushToolDiameter > 4 && (x1 == x0 || abs(x1 - x0) % (mBrushToolDiameter / 4) == 0)) {
				if (!doselection)
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
			} else if (doselection || mBrushToolDiameter < 4) {
				if (!doselection)
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				else
					PickToolAddToSelection(segmentation_id, globalDC);
			}
			// //debug("FIXME", << "insert: " << DataCoord(x0, y0, 0) << endl;
		}
	} else {
		int fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			//modifiedCoords.insert(DataCoord(x0, y0, mViewDepth));
			DataCoord myDC = DataCoord(x0, y0, mViewDepth);

			// myDC is flat, only valid for XY view.  This is not correct.

			DataCoord globalDC;
			switch (mViewType) {
			case XY_VIEW:{
					globalDC = DataCoord(myDC.x, myDC.y, myDC.z);
					//                                      //debug("FIXME", << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
				}
				break;
			case XZ_VIEW:{
					globalDC = DataCoord(myDC.x, myDC.z, myDC.y);
				}
				break;
			case YZ_VIEW:{
					globalDC = DataCoord(myDC.z, myDC.y, myDC.x);
				}
				break;
			}
			//debug("FIXME", << "global click point: " << globalDC << endl;

			if (mBrushToolDiameter > 4 && (y1 == y0 || abs(y1 - y0) % (mBrushToolDiameter / 4) == 0)) {
				if (!doselection)
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
			} else if (doselection || mBrushToolDiameter < 4) {
				if (!doselection)
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				else
					PickToolAddToSelection(segmentation_id, globalDC);
			}
			// //debug("FIXME", << "insert: " << DataCoord(x0, y0, 0) << endl;
		}
	}

}

void OmView2d::GlobalDepthFix(float howMuch)
{
	float depth = OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW) * howMuch;
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, depth));
	int dataDepth = data_coord.z;
	SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, dataDepth));
	OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, space_coord.z, false);

	depth = OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW) * howMuch;
	data_coord = SpaceToDataCoord(SpaceCoord(0, 0, depth));
	dataDepth = data_coord.z;
	space_coord = DataToSpaceCoord(DataCoord(0, 0, dataDepth));
	OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, space_coord.z), false;

	depth = OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW) * howMuch;
	data_coord = SpaceToDataCoord(SpaceCoord(0, 0, depth));
	dataDepth = data_coord.z;
	space_coord = DataToSpaceCoord(DataCoord(0, 0, dataDepth));
	OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, space_coord.z, false);
}

#pragma mark
#pragma mark KeyEvent Methods
/////////////////////////////////
///////          KeyEvent Methods

void OmView2d::wheelEvent(QWheelEvent * event)
{
	//debug("genone","OmView2d::wheelEvent -- " << mViewType);

	const int numDegrees = event->delta() / 8;
	const int numSteps = numDegrees / 15;

	const bool move_through_stack = event->modifiers() & Qt::ControlModifier;

	if (move_through_stack) {
		if (numSteps >= 0) {
			MoveUpStackCloserToViewer();
		} else {
			MoveDownStackFartherFromViewer();
		}
	} else {
		MouseWheelZoom(numSteps);
	}

	event->accept();

	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}

void OmView2d::MouseWheelZoom(const int numSteps)
{
	if (numSteps >= 0) {
		// ZOOMING IN

		Vector2 < int >current_zoom = OmStateManager::Instance()->GetZoomLevel();

		if (!mLevelLock && (current_zoom.y >= 10) && (current_zoom.x > 0)) {
			// need to move to previous mip level
			OmStateManager::Instance()->SetZoomLevel(Vector2 < int >(current_zoom.x - 1, 6));

			GlobalDepthFix(2.0);
		} else
			OmStateManager::Instance()->SetZoomLevel(Vector2 <
								 int >(current_zoom.x,
								       current_zoom.y + (1 * numSteps)));

		PanOnZoom(current_zoom);
	} else {
		// ZOOMING OUT

		Vector2 < int >current_zoom = OmStateManager::Instance()->GetZoomLevel();

		if (!mLevelLock && (current_zoom.y <= 6) && (current_zoom.x < mRootLevel)) {
			// need to move to next mip level
			OmStateManager::Instance()->SetZoomLevel(Vector2 < int >(current_zoom.x + 1, 10));

			GlobalDepthFix(0.5);
		}

		else if (current_zoom.y > 1) {
			int zoom = current_zoom.y - (1 * (-1 * numSteps));
			if (zoom < 1)
				zoom = 1;

			OmStateManager::Instance()->SetZoomLevel(Vector2 < int >(current_zoom.x, zoom));
		}

		PanOnZoom(current_zoom);
	}
}

void OmView2d::SetViewSliceOnPan()
{
	Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();

	float pl = pow(2, zoomMipVector.x);
	float scaleFactor = zoomMipVector.y / 10.0;

	Vector2i minCoord =
	    Vector2i(mTotalViewport.lowerLeftX - translateVector.x * pl,
		     mTotalViewport.lowerLeftY - translateVector.y * pl);
	SpaceCoord minSpaceCoord = DataToSpaceCoord(DataCoord(minCoord.x, minCoord.y, 0));
	Vector2i maxCoord =
	    Vector2i(mTotalViewport.width / scaleFactor * pl - translateVector.x * pl,
		     mTotalViewport.height / scaleFactor * pl - translateVector.y * pl);
	SpaceCoord maxSpaceCoord = DataToSpaceCoord(DataCoord(maxCoord.x, maxCoord.y, 0));

	if (mViewType == YZ_VIEW) {
		OmStateManager::SetViewSliceMax(mViewType, Vector2f(maxSpaceCoord.y, maxSpaceCoord.x), false);
		OmStateManager::SetViewSliceMin(mViewType, Vector2f(minSpaceCoord.y, minSpaceCoord.x), false);
	} else {
		OmStateManager::SetViewSliceMax(mViewType, Vector2f(maxSpaceCoord.x, maxSpaceCoord.y), false);
		OmStateManager::SetViewSliceMin(mViewType, Vector2f(minSpaceCoord.x, minSpaceCoord.y), false);
	}
}

void OmView2d::PanOnZoomSelf(Vector2 < int >current_zoom)
{
	Vector2 < int >pro_zoom = OmStateManager::Instance()->GetZoomLevel();
	int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
	int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;

	if (pro_zoom.x > current_zoom.x) {
		widthTranslate = widthTranslate / 2;
		heightTranslate = heightTranslate / 2;
	} else if (pro_zoom.x < current_zoom.x) {
		widthTranslate = widthTranslate * 2;
		heightTranslate = heightTranslate * 2;
	}
	OmStateManager::Instance()->SetPanDistance(mViewType, Vector2 < int >(widthTranslate, heightTranslate), false);
}

void OmView2d::PanOnZoom(Vector2 < int >current_zoom, bool postEvent)
{

	// Update the pan so view stays centered.
	ViewType vts[] = { XY_VIEW, YZ_VIEW, XZ_VIEW };

	for (int i = 0; i < 3; i++) {
		Vector2 < int >pro_zoom = OmStateManager::Instance()->GetZoomLevel();
		int widthTranslate = OmStateManager::Instance()->GetPanDistance(vts[i]).x;
		int heightTranslate = OmStateManager::Instance()->GetPanDistance(vts[i]).y;

		if (pro_zoom.x > current_zoom.x) {
			widthTranslate = widthTranslate / 2;
			heightTranslate = heightTranslate / 2;
		} else if (pro_zoom.x < current_zoom.x) {
			widthTranslate = widthTranslate * 2;
			heightTranslate = heightTranslate * 2;
		}

		OmStateManager::Instance()->SetPanDistance(vts[i], Vector2 < int >(widthTranslate, heightTranslate),
							   postEvent);
	}

	SetViewSliceOnPan();
}

void OmView2d::setBrushToolDiameter()
{
	if (1 == mBrushToolDiameter) {
		mBrushToolDiameter = 2;
	} else if (2 == mBrushToolDiameter) {
		mBrushToolDiameter = 8;
	} else if (8 == mBrushToolDiameter) {
		mBrushToolDiameter = 32;
	} else if (32 == mBrushToolDiameter) {
		mBrushToolDiameter = 1;
	}

	/*
	   if (1 == mBrushToolDiameter) {
	   mBrushToolDiameter = 2;
	   } else if (2 == mBrushToolDiameter) {
	   mBrushToolDiameter = 4;
	   } else if (4 == mBrushToolDiameter) {
	   mBrushToolDiameter = 8;
	   } else if (8 == mBrushToolDiameter) {
	   mBrushToolDiameter = 16;             
	   } else if (16 == mBrushToolDiameter) {
	   mBrushToolDiameter = 32;             
	   } else if (32 == mBrushToolDiameter) {
	   mBrushToolDiameter = 1;              
	   }
	 */
}

void OmView2d::keyPressEvent(QKeyEvent * event)
{
	//debug("genone","OmView2d::keyPressEvent -- " << mViewType);

	switch (event->key()) {
	case Qt::Key_A:
		{
			if (mAnimation) {
				mAnimation->safeTerminate ();
				mAnimation = NULL;
			} else {
				mAnimation = new OmAnimate (1);
				mAnimation->start ();
			}
		}
	case Qt::Key_D:
		{
			mMIP = !mMIP;
		}
		break;
	case Qt::Key_M:
		{
			mEmitMovie = !mEmitMovie;
		}
		break;
	case Qt::Key_P:
		{
			setBrushToolDiameter();
			myUpdate();
		}
		break;
	case Qt::Key_L:
		{
			mLevelLock = !mLevelLock;
			myUpdate();
		}
		break;
	case Qt::Key_J:
		// Toggle joining of a segment.
		{
			bool domerge = false;
			OmId seg;
			if (mVolumeType == SEGMENTATION) {
				domerge = true;
				seg = mImageId;
			} else {
				OmChannel & current_channel = OmVolume::GetChannel(mImageId);
				const set < OmId > objectIDs = current_channel.GetValidFilterIds();
				set < OmId >::iterator obj_it;

                		for( obj_it=objectIDs.begin(); obj_it != objectIDs.end(); obj_it++ ) {
                        		OmFilter2d &filter = current_channel.GetFilter(*obj_it);
                        		seg = filter.GetSegmentation ();
                        		if (seg) {
						domerge = true;
						break;
					}
				}
			}

			if (domerge) {
				OmSegmentation & current_seg = OmVolume::GetSegmentation(seg);
				(new OmSegmentMergeAction(seg))->Run();
				current_seg.SetAllSegmentsSelected(false);
				Refresh();
				mTextures.clear();
			}

			myUpdate();
		}
		break;

	case Qt::Key_F:
		// Toggle fill mode.
		{
			myUpdate();
		}
		break;
	case Qt::Key_Escape:
		{
			// Attempt to return to a safe camera location.
			mLevelLock = false;
			Vector2 < int >current_zoom = OmStateManager::Instance()->GetZoomLevel();
			current_zoom.x = 0;
			current_zoom.y = 10;
			OmStateManager::Instance()->SetZoomLevel(current_zoom);

			int widthTranslate = 0;
			int heightTranslate = 0;

			OmStateManager::Instance()->SetPanDistance(mViewType,
								   Vector2 < int >(widthTranslate, heightTranslate));

			Refresh();
			mTextures.clear();
			myUpdate();

		}
		break;
	case Qt::Key_Minus:
		{
			Vector2 < int >current_zoom = OmStateManager::Instance()->GetZoomLevel();

			if (!mLevelLock && (current_zoom.y == 6) && (current_zoom.x < mRootLevel)) {
				OmStateManager::Instance()->SetZoomLevel(Vector2 < int >(current_zoom.x + 1, 10));

				GlobalDepthFix(0.5);
			}

			else if (current_zoom.y > 1)
				OmStateManager::Instance()->SetZoomLevel(Vector2 <
									 int >(current_zoom.x, current_zoom.y - 1));

			PanOnZoom(current_zoom);

		}
		break;
	case Qt::Key_Equal:
		{
			Vector2 < int >current_zoom = OmStateManager::Instance()->GetZoomLevel();

			if (!mLevelLock && (current_zoom.y == 10) && (current_zoom.x > 0)) {
				OmStateManager::Instance()->SetZoomLevel(Vector2 < int >(current_zoom.x - 1, 6));
				GlobalDepthFix(2.0);
			} else
				OmStateManager::Instance()->SetZoomLevel(Vector2 <
									 int >(current_zoom.x, current_zoom.y + 1));

			PanOnZoom(current_zoom);
		}
		break;
	case Qt::Key_Right:
		{
			Vector2 < int >current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);

			OmStateManager::Instance()->SetPanDistance(mViewType,
								   Vector2 < int >(current_pan.x + 5, current_pan.y));

			SetViewSliceOnPan();

		}
		break;
	case Qt::Key_Left:
		{
			Vector2 < int >current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);

			OmStateManager::Instance()->SetPanDistance(mViewType,
								   Vector2 < int >(current_pan.x - 5, current_pan.y));

			SetViewSliceOnPan();
		}
		break;
	case Qt::Key_Up:
		{
			Vector2 < int >current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);

			OmStateManager::Instance()->SetPanDistance(mViewType,
								   Vector2 < int >(current_pan.x, current_pan.y + 5));

			SetViewSliceOnPan();
		}
		break;
	case Qt::Key_Down:
		{
			Vector2 < int >current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);

			OmStateManager::Instance()->SetPanDistance(mViewType,
								   Vector2 < int >(current_pan.x, current_pan.y - 5));

			SetViewSliceOnPan();
		}
		break;
	case Qt::Key_W:
	case Qt::Key_PageUp:
		{
			MoveUpStackCloserToViewer();

		}
		break;
	case Qt::Key_S:
	case Qt::Key_PageDown:
		{
			MoveDownStackFartherFromViewer();
		}
		break;

	default:
		QWidget::keyPressEvent(event);
	}
}

void OmView2d::MoveUpStackCloserToViewer()
{
	const float Depth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, Depth));

	const int ViewDepth = data_coord.z;

	SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, ViewDepth + 1));

	OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);
}

void OmView2d::MoveDownStackFartherFromViewer()
{
	const float Depth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, Depth));

	const int ViewDepth = data_coord.z;

	SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, ViewDepth - 1));

	OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);
}

#pragma mark
#pragma mark OmEvent Methods
/////////////////////////////////
///////          OmEvent Methods

void OmView2d::PreferenceChangeEvent(OmPreferenceEvent * event)
{

	switch (event->GetPreference()) {
		
	case OM_PREF_VIEW2D_SHOW_INFO_BOOL:
		myUpdate();
		break;
	case OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT:
		mCache->SetNewAlpha(OmPreferences::GetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT));
		myUpdate();
		break;
	case OM_PREF_VIEW2D_VOLUME_CACHE_SIZE_INT:
		break;
	case OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT:
		myUpdate();
		break;
	case OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT:
		depthCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT);
		break;
	case OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT:
		sidesCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT);
		break;
	case OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT:
		mipCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT);
		break;
	default:
		return;
	}
}

/*
 * Redraw this widget
 */

void OmView2d::SegmentObjectModificationEvent(OmSegmentEvent * event)
{
	//add/remove segment, change state, change selection
	//valid methods: GetModifiedSegmentIds()

	debug("genone","OmView2d::SegmentObjectModificationEvent\n");

	Refresh ();
	myUpdate();
}

void OmView2d::SegmentDataModificationEvent(OmSegmentEvent * event)
{
	//voxels of a segment have changed
	//valid methods: GetModifiedSegmentIds()

	debug("genone","OmView2d::SegmentDataModificationEvent\n");
}

void OmView2d::SegmentEditSelectionChangeEvent(OmSegmentEvent * event)
{
	//change segment edit selection

	debug("genone","OmView2d::SegmentEditSelectionChangeEvent\n");

	if (mVolumeType == SEGMENTATION) {
		//              modified_Ids = event->GetModifiedSegmentIds();
		//              delete_dirty = true;
		//              myUpdate();

		// need to myUpdate paintbrush, not anything on the screen 

		OmId mentationEditId;
		OmId mentEditId;

		if (OmSegmentEditor::GetEditSelection(mentationEditId, mentEditId)) {

			if (mentationEditId == mImageId) {

				const Vector3 < float >&color =
				    OmVolume::GetSegmentation(mentationEditId).GetSegment(mentEditId).GetColor();

				////debug("genone","SETTING EDIT COLOR");
				editColor = qRgba(color.x * 255, color.y * 255, color.z * 255, 255);
			}
		}
	}
}

void OmView2d::VoxelModificationEvent(OmVoxelEvent * event)
{
	//voxels in a segmentation have been modified
	//valid methods: GetSegmentationId(), GetVoxels()

	//debug("genone","OmView2d::VoxelModificationEvent() ---" << mViewType);

	// //debug("FIXME", << "voxel modification event" << endl;
	//debug("genone","event segmentation id = " << event->GetSegmentationId());
	//debug("genone","iSentIt = " << iSentIt);
	//modifiedCoords.clear();
	// //debug("FIXME", << "event segmentation id: " << event->GetSegmentationId() << endl;
	if ((mVolumeType == SEGMENTATION) && (event->GetSegmentationId() == mImageId)) {

		set < DataCoord > modVoxels = event->GetVoxels();

		// these voxels are not flat, they are correct for ortho views

		set < DataCoord >::iterator itr;

		for (itr = modVoxels.begin(); itr != modVoxels.end(); itr++) {
			//debug("genone","data coord = " << *itr);
			modifiedCoords.insert(*itr);

		}

		myUpdate();
	}
}

void OmView2d::SystemModeChangeEvent(OmSystemModeEvent * event)
{
	//debug("genone","OmView2d::SystemModeChangeEvent");

	if (mVolumeType == SEGMENTATION) {
		modified_Ids = OmVolume::GetSegmentation(mImageId).GetSelectedSegmentIds();
		delete_dirty = true;
		myUpdate();
	}

}

#pragma mark
#pragma mark Actions
/////////////////////////////////
///////          Actions

#pragma mark
#pragma mark ViewEvent Methods
/////////////////////////////////
///////          ViewEvent Methods

void OmView2d::ViewBoxChangeEvent(OmViewEvent * event)
{
	//debug("genone","OmView2d::ViewBoxChangeEvent -- " << mViewType);
	myUpdate();
}

void OmView2d::ViewPosChangeEvent(OmViewEvent * event)
{
	myUpdate();
}

void OmView2d::ViewCenterChangeEvent(OmViewEvent * event)
{
        DataCoord data_coord;
	float fdepth;
	int realydepth, realxdepth;


	fdepth = OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW);
        data_coord = SpaceToDataCoord(SpaceCoord(0, 0, fdepth));
        int xdepth = data_coord.z;

	fdepth = OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW);
	data_coord = SpaceToDataCoord(SpaceCoord(0, 0, fdepth));
	int ydepth = data_coord.z;

	fdepth = OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW);
	data_coord = SpaceToDataCoord(SpaceCoord(0, 0, fdepth));
	int zdepth = data_coord.z;

	debug ("view2d", "view: %i  depth x,y,z (%i, %i, %i)\n", mViewType,xdepth, ydepth, zdepth);
	

	if (YZ_VIEW == mViewType) {
		realxdepth = zdepth;
		realydepth = ydepth;
	} else if (XZ_VIEW == mViewType) {
		realxdepth = xdepth;
		realydepth = zdepth;
	} else if (XY_VIEW == mViewType) {
		realxdepth = xdepth;
		realydepth = ydepth;
	}

	// TODO: zoom factors other than 1 don't seem to work
        // Update the pan so view stays centered.
        Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
        float scaleFactor = (zoomMipVector.y / 10.);
	debug("view2d","view: %i  scaleFactor: %f \n",mViewType,scaleFactor);
        Vector2 <int > current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
        int shiftx = (int)((float)(mTotalViewport.width/2/scaleFactor-realxdepth-current_pan.x));
	int shifty = (int)((float) (mTotalViewport.height/2/scaleFactor-realydepth-current_pan.y));
	debug("view2d","view: %i mTotalViewport.width: %i  (w=mTotalViewport.width/2): %i \n",mViewType,mTotalViewport.width,mTotalViewport.width/2);
	debug("view2d","view: %i  (w-current_pan.x)/sf: %i realxdepth: %i  \n", mViewType,(int)((float)( mTotalViewport.width/2-current_pan.x)/ scaleFactor),realxdepth);
	debug("view2d","view: %i realydepth %i \n",mViewType,realydepth);
	debug("view2d","view: %i  (h-current_pan.y)/sf: %i realydepth: %i  \n", mViewType,(int)((float)( mTotalViewport.height/2-current_pan.y)/ scaleFactor),realydepth);
	debug("view2d","view: %i  current_pan.x: %i   shiftx: %i  \n", mViewType,current_pan.x,shiftx);
	debug("view2d","view: %i  current_pan.y: %i   shifty: %i  \n", mViewType,current_pan.y,shifty);
        OmStateManager::Instance()->SetPanDistance(mViewType,
                                                   Vector2 < int >(current_pan.x + shiftx, current_pan.y + shifty));

	myUpdate();
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}

void OmView2d::ViewRedrawEvent(OmViewEvent * event)
{
	myUpdate();
}

void OmView2d::myUpdate()
{

	if (mEditedSegmentation) {
		(new OmVoxelSetValueAction(mEditedSegmentation, mUpdateCoordsSet, mCurrentSegmentId))->Run();
		mUpdateCoordsSet.clear();
	}

	if (!mDoRefresh && mEditedSegmentation) {
		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
		int tileLength = OmVolume::GetSegmentation(mEditedSegmentation).GetChunkDimension();
		OmSegmentation & current_seg = OmVolume::GetSegmentation(mEditedSegmentation);

		OmCachingThreadedCachingTile *fastCache =
		    new OmCachingThreadedCachingTile(mViewType, SEGMENTATION, mEditedSegmentation, &current_seg, NULL);
		OmThreadedCachingTile *cache = fastCache->mCache;
		if (fastCache->mDelete)
			delete fastCache;

#if 0
		//debug("FIXME", << "enter" << endl;
		//debug("FIXME", << "mBrushToolMinX " << mBrushToolMinX << endl;
		//debug("FIXME", << "mBrushToolMinY " << mBrushToolMinY << endl;
		//debug("FIXME", << "mBrushToolMinZ " << mBrushToolMinZ << endl;
		//debug("FIXME", << "mBrushToolMaxZ " << mBrushToolMaxZ << endl;
		//debug("FIXME", << "enter" << endl;
#endif

		mTextures.clear();

		int xMipChunk, xSave = -1;
		int yMipChunk, ySave = -1;
		int zMipChunk, zSave = -1;
		int step = 1;
		for (int x = mBrushToolMinX; x <= mBrushToolMaxX; x = x + step) {
			for (int y = mBrushToolMinY; y <= mBrushToolMaxY; y = y + step) {
				for (int z = mBrushToolMinZ; z <= mBrushToolMaxZ; z = z + step) {

					if (mViewType == XY_VIEW) {
						xMipChunk = ((int)(x / tileLength)) * tileLength;
						yMipChunk = ((int)(y / tileLength)) * tileLength;
						zMipChunk = z;
					} else if (mViewType == XZ_VIEW) {
						xMipChunk = ((int)(x / tileLength)) * tileLength;
						yMipChunk = ((int)(z / tileLength)) * tileLength;
						zMipChunk = y;
					} else if (mViewType == YZ_VIEW) {
						xMipChunk = ((int)(z / tileLength)) * tileLength;
						yMipChunk = ((int)(y / tileLength)) * tileLength;
						zMipChunk = x;
					}

					if (xMipChunk != xSave || yMipChunk != ySave || zMipChunk != zSave) {
						DataCoord this_data_coord =
						    ToDataCoord(xMipChunk, yMipChunk, zMipChunk);
						SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
						OmTileCoord tileCoord = OmTileCoord(zoomMipVector.x, this_space_coord, SEGMENTATION, OmCachingThreadedCachingTile::Freshen (false));

						cache->Remove(tileCoord);
						//debug("FIXME", << tileCoord << endl;
						//debug("FIXME", << "here " << endl;
					}
					//debug("FIXME", << " x,y,z a:" << x << ", "  << y << ", " << z << endl;

					xSave = xMipChunk;
					ySave = yMipChunk;
					zSave = zMipChunk;
				}
			}
		}
		//debug("FIXME", << "exit" << endl;
	} else if (mDoRefresh) {
		OmCachingThreadedCachingTile::Refresh();
		mDoRefresh = false;
	}

	mBrushToolMaxX = 0;
	mBrushToolMaxY = 0;
	mBrushToolMaxZ = 0;
	mBrushToolMinX = INT_MAX;
	mBrushToolMinY = INT_MAX;
	mBrushToolMinZ = INT_MAX;

	update();
}

#pragma mark
#pragma mark Draw Methods
/////////////////////////////////
///////		 Draw Methods
void OmView2d::DrawFromFilter(OmFilter2d &filter)
{
	OmThreadedCachingTile *cache = filter.GetCache(mViewType);
	if (!cache)
		return;

	OmThreadedCachingTile *sCache = mCache;
	mCache = cache;
	double alpha = mAlpha;
	mAlpha = filter.GetAlpha();
	mCurrentSegmentation = filter.GetSegmentation();

	Draw(false);

	mAlpha = alpha;
	mCache = sCache;
}

void OmView2d::DrawFromCache()
{
	if (mVolumeType == CHANNEL) {
		OmChannel & current_channel = OmVolume::GetChannel(mImageId);
		mVolume = &current_channel;

		OmCachingThreadedCachingTile *fastCache =
		    new OmCachingThreadedCachingTile(mViewType, mVolumeType, mImageId, &current_channel, NULL);
		mCache = fastCache->mCache;
		if (fastCache->mDelete)
			delete fastCache;

		mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);
		mCache->SetContinuousUpdate(false);

		Draw(true);
	} else {
		mCurrentSegmentation = mImageId;
		OmSegmentation & current_seg = OmVolume::GetSegmentation(mImageId);
		mVolume = &current_seg;

		OmCachingThreadedCachingTile *fastCache =
		    new OmCachingThreadedCachingTile(mViewType, mVolumeType, mImageId, &current_seg, NULL);
		mCache = fastCache->mCache;
		if (fastCache->mDelete)
			delete fastCache;

		mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);
		mCache->SetContinuousUpdate(false);
		Draw(false);
	}

}

extern GGOCTFPointer GGOCTFunction;

void OmView2d::safeTexture(shared_ptr < OmTextureID > gotten_id)
{
	if (OMTILE_NEEDCOLORMAP == gotten_id->flags) {
		GLuint texture;
		glGenTextures(1, &texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gotten_id->x, gotten_id->y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			     gotten_id->texture);

		gotten_id->flags = OMTILE_GOOD;
		gotten_id->textureID = texture;
		if (gotten_id->texture) {
			//debug ("genone", "freeing texture: %x\n", gotten_id->texture);
			free(gotten_id->texture);
		}
		gotten_id->texture = NULL;

		debug("genone", "texture = %i\n", gotten_id->GetTextureID());

	} else if (OMTILE_NEEDTEXTUREBUILT == gotten_id->flags) {
		GLuint texture;
		glGenTextures(1, &texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		//debug("FIXME", << "texture " << (int)((unsigned char*)gotten_id->texture)[0] << endl;
		//debug("FIXME", << "x " << gotten_id->x << endl;
		//debug("FIXME", << "y " << gotten_id->y << endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, gotten_id->x, gotten_id->y, 0, GL_LUMINANCE,
			     GL_UNSIGNED_BYTE, ((unsigned char *)gotten_id->texture));

		gotten_id->flags = OMTILE_GOOD;
		gotten_id->textureID = texture;

		if (gotten_id->texture) {
			//debug ("genone", "freeing texture: %x\n", gotten_id->texture);
			free(gotten_id->texture);
		}
		gotten_id->texture = NULL;
	}
}

void OmView2d::safeDraw(float zoomFactor, int x, int y, int tileLength, shared_ptr < OmTextureID > gotten_id)
{
	if (mViewType == YZ_VIEW) {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.5, 0.5, 0.0);
		glRotatef(-90, 0.0, 0.0, 1.0);
		glTranslatef(-0.5, -0.5, 0.0);
		glMatrixMode(GL_MODELVIEW);
	}

	glBindTexture(GL_TEXTURE_2D, gotten_id->GetTextureID());
	glBegin(GL_QUADS);

	if (mViewType == XY_VIEW) {
		glTexCoord2f(0.0f, 0.0f);	/* lower left corner of image */
		glVertex2f(x * zoomFactor, y * zoomFactor);

		glTexCoord2f(1.0f, 0.0f);	/* lower right corner of image */
		glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);

		glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
		glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);

		glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
		glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);
		glEnd();
	} else if (mViewType == XZ_VIEW) {
		glTexCoord2f(0.0f, 0.0f);	/* lower left corner of image */
		glVertex2f(x * zoomFactor, y * zoomFactor);

		glTexCoord2f(1.0f, 0.0f);	/* lower right corner of image */
		glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);

		glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
		glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);

		glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
		glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);
		glEnd();
	} else if (mViewType == YZ_VIEW) {
		glTexCoord2f(0.0f, 0.0f);	/* lower left corner of image */
		glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);

		glTexCoord2f(1.0f, 0.0f);	/* lower right corner of image */
		glVertex2f(x * zoomFactor, y * zoomFactor);

		glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
		glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);

		glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
		glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);
		glEnd();
	}
}

DataCoord OmView2d::ToDataCoord(int xMipChunk, int yMipChunk, int mDataDepth)
{
	DataCoord this_data_coord;
	switch (mViewType) {
	case XY_VIEW:
		this_data_coord = DataCoord(xMipChunk, yMipChunk, mDataDepth);
		break;
	case XZ_VIEW:
		this_data_coord = DataCoord(xMipChunk, mDataDepth, yMipChunk);
		break;
	case YZ_VIEW:
		this_data_coord = DataCoord(mDataDepth, yMipChunk, xMipChunk);
		break;
	}
	return this_data_coord;
}

void OmView2d::Draw(int mip)
{
	drawComplete = true;

	Vector2f zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	if (mip) {
		Vector2f zoom = zoomMipVector;
		Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);

		int lvl = zoomMipVector.x + 2;

		for (int i = mRootLevel; i > lvl; i--) {
			PanOnZoomSelf(zoom);
			zoom.x = i;
			zoom.y = zoomMipVector.y * (1 + i - zoomMipVector.x);
			debug("genone","OmView2d::Draw(zoom lvl %i, scale %i\n)\n");

			OmStateManager::Instance()->SetPanDistance(mViewType,
								   Vector2 <
								   int >(translateVector.x / (1 + i - zoomMipVector.x),
									 translateVector.y / (1 + i - zoomMipVector.x)),
								   false);

			PreDraw(zoom);
		}
		OmStateManager::Instance()->SetPanDistance(mViewType,
							   Vector2 < int >(translateVector.x, translateVector.y),
							   false);
	}

	PreDraw(zoomMipVector);

	Draw(mTextures);
	mTextures.clear ();
}

Drawable::Drawable(int x, int y, int tileLength, OmTileCoord tileCoord, float zoomFactor,
		   shared_ptr < OmTextureID > gotten_id)
:x(x), y(y), tileLength(tileLength), tileCoord(tileCoord), zoomFactor(zoomFactor), gotten_id(gotten_id)
{
	mGood = true;
}

Drawable::Drawable(int x, int y, int tileLength, OmTileCoord tileCoord, float zoomFactor)
:x(x), y(y), tileLength(tileLength), tileCoord(tileCoord), zoomFactor(zoomFactor)
{
	mGood = false;
}
Drawable::~Drawable ()
{
	//debug ("genone", "freeing?\n");
	gotten_id = shared_ptr < OmTextureID > ();
}

void OmTextureIDUpdate(shared_ptr < OmTextureID > gotten_id, const OmTileCoord tileCoord, const GLuint texID,
		       const int size, int x, int y, const OmIds & containedIds, void *texture, int flags)
{
	//debug("FIXME", << "in OmTextureIDUpdate" << endl;

	if (gotten_id->texture) {
		//debug ("genone", "freeing texture: %x\n", gotten_id->texture);
		free (gotten_id->texture);
	}
	//debug ("genone", "updating texture: %x to be %x\n", gotten_id->texture, texture);

	gotten_id->mTileCoordinate = tileCoord;
	gotten_id->textureID = texID;
	gotten_id->mem_size = size;
	gotten_id->mIdSet = containedIds;
	gotten_id->flags = flags;
	gotten_id->texture = texture;
	gotten_id->x = x;
	gotten_id->y = y;
}

int OmView2d::GetDepth(const OmTileCoord & key, OmMipVolume * vol)
{
	// find depth
	NormCoord normCoord = OmVolume::SpaceToNormCoord(key.Coordinate);
	DataCoord dataCoord = OmVolume::NormToDataCoord(normCoord);

	if (mViewType == XY_VIEW) {
		return dataCoord.z;
	}
	if (mViewType == XZ_VIEW) {
		return dataCoord.y;
	}
	if (mViewType == YZ_VIEW) {
		return dataCoord.x;
	}
}

static int clamp(int c)
{
	if (c > 255)
		return 255;
	return c;
}

OmIds OmView2d::setMyColorMap(OmId segid, SEGMENT_DATA_TYPE * imageData, Vector2 < int >dims, const OmTileCoord & key,
			      void **rData)
{
	debug("genone","OmTile::setMyColorMap(imageData=%i)\n", imageData);

	OmIds found_ids;
	bool entered;

	DataBbox data_bbox = mCache->mVolume->MipCoordToDataBbox(mCache->TileToMipCoord(key), 0);
	int my_depth = GetDepth(key, mCache->mVolume);

	unsigned char *data = new unsigned char[dims.x * dims.y * SEGMENT_DATA_BYTES_PER_SAMPLE];
	//bzero (data, dims.x*dims.y*SEGMENT_DATA_BYTES_PER_SAMPLE);

	int ctr = 0;
	int newctr = 0;
	SEGMENT_DATA_TYPE lastid = 0;

	OmSegmentation & current_seg = OmVolume::GetSegmentation(segid);
	bool doValidate = current_seg.GetSelectedSegmentDataValues ().size ();
        if (SEGMENTATION == key.mVolType) {
                doValidate = false;
        }

        //debug ("genone", "key volume type, real: %i\n", key.mVolType, mVolumeType);


	QHash < SEGMENT_DATA_TYPE, QColor > speedTable;
	QColor newcolor;
	OmId id;

	// looping through each value of imageData, which is strictly dims.x * dims.y big, no extra because of cast to SEGMENT_DATA_TYPE
	for (int i = 0; i < dims.x * dims.y; i++) {
		SEGMENT_DATA_TYPE tmpid = (SEGMENT_DATA_TYPE) imageData[i];

		//if (0 != tmpid) debug("genone", "gotten segment id %i\n", tmpid);

		if (tmpid != lastid) {
			if (!speedTable.contains(tmpid)) {

				//debug("genone", "gotten segment id %i mapped to value\n", tmpid);

				OmId id = current_seg.GetSegmentIdMappedToValue(tmpid);
				if (id == 0) {
					data[ctr] = 0;
					data[ctr + 1] = 0;
					data[ctr + 2] = 0;
					data[ctr + 3] = 255;
					newcolor = qRgba(0, 0, 0, 255);
				} else {

					// //debug("FIXME", << "asking for color now" << endl;
					const Vector3 < float >&color =
					    OmVolume::GetSegmentation(segid).GetSegment(id).GetColor();

					if (current_seg.IsSegmentSelected(id)) {

						newcolor =
						    qRgba(clamp(color.x * 255 * 2.75), clamp(color.y * 255 * 2.75),
							  clamp(color.z * 255 * 2.75), 100);
						entered = true;

					} else {
						if (doValidate)  {
							newcolor = qRgba(0, 0, 0, 255);
						} else {
							//debug ("genone", "validate functionality not used\n");
							newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);
						}
					}

					data[ctr] = newcolor.red();
					data[ctr + 1] = newcolor.green();
					data[ctr + 2] = newcolor.blue();
					data[ctr + 3] = 255;

				}

				speedTable[tmpid] = newcolor;
				//debug("FIXME", << " adding to speed table" << endl;

			} else {
				//debug("FIXME", << " using speed table" << endl;
				newcolor = speedTable.value(tmpid);
				data[ctr] = newcolor.red();
				data[ctr + 1] = newcolor.green();
				data[ctr + 2] = newcolor.blue();
				data[ctr + 3] = 255;
			}
		} else {
			data[ctr] = newcolor.red();
			data[ctr + 1] = newcolor.green();
			data[ctr + 2] = newcolor.blue();
			data[ctr + 3] = 255;
		}
		newctr = newctr + 1;
		ctr = ctr + 4;
		lastid = tmpid;
	}

	free (imageData);

	*rData = data;

	return found_ids;
}

void *OmView2d::GetImageData(const OmTileCoord & key, Vector2 < int >&sliceDims, OmMipVolume * vol)
{
	//debug("FIXME", << "in OmView2d::GetImageData" << endl;
	shared_ptr < OmMipChunk > my_chunk;
	vol->GetChunk(my_chunk, mCache->TileToMipCoord(key));
	int depth = GetDepth(key, vol);
	int realDepth = depth % (vol->GetChunkDimension());

	void *void_data = NULL;
	if (mViewType == XY_VIEW) {
		void_data = my_chunk->ExtractDataSlice(VOL_XY_PLANE, realDepth, sliceDims, true);
	} else if (mViewType == XZ_VIEW) {
		void_data = my_chunk->ExtractDataSlice(VOL_XZ_PLANE, realDepth, sliceDims, true);
	} else if (mViewType == YZ_VIEW) {
		void_data = my_chunk->ExtractDataSlice(VOL_YZ_PLANE, realDepth, sliceDims, true);
	}
	//debug ("genone", "allocating texture %x\n", void_data);
	return void_data;
}

void OmView2d::myBindToTextureID(shared_ptr < OmTextureID > gotten_id)
{
	//std::cerr << "entering " << __FUNCTION__ << endl;
	int BPS = mCache->mVolume->GetBytesPerSample();
	OmMipChunkCoord coord = mCache->TileToMipCoord(gotten_id->mTileCoordinate);

	//debug("FIXME", << "tile coord: "  << gotten_id->mTileCoordinate << endl;

	if (mCache->mVolume->ContainsMipChunkCoord(coord)) {

		int mcc_x = coord.Coordinate.x;
		int mcc_y = coord.Coordinate.y;
		int mcc_z = coord.Coordinate.z;

		void *vData = NULL;
		if ((mcc_x >= 0) && (mcc_y >= 0) && (mcc_z >= 0)) {
			Vector2 < int >tile_dims;
			vData = GetImageData(gotten_id->mTileCoordinate, tile_dims, mCache->mVolume);

			if (!vData) {
				assert (0);
				return;
			}

			//debug("FIXME", << "vData: "  << vData << endl;
			if (mCache->mVolType == CHANNEL) {
				OmIds myIdSet;
				//debug("genone", "CALL1 %x\n", vData);
				OmTextureIDUpdate(gotten_id, gotten_id->mTileCoordinate, 0, (tile_dims.x * tile_dims.y),
						  tile_dims.x, tile_dims.y, myIdSet, vData, OMTILE_NEEDTEXTUREBUILT);
			} else {
				if (1 == BPS) {
					//debug("genone", "1 bps !!!!!!!!!!!!!!!!!\n");
					uint32_t *vDataFake;
					vDataFake =
					    (uint32_t *) malloc((tile_dims.x * tile_dims.y) *
								sizeof(SEGMENT_DATA_TYPE));
					for (int i = 0; i < (tile_dims.x * tile_dims.y); i++) {
						vDataFake[i] = ((unsigned char *)(vData))[i];
						vDataFake[i] << 8;
						//debug("FIXME", << " "  << (int)((unsigned char*)(vData))[i];
					}
					//debug("FIXME", << endl;
					free(vData);
					vData = (void *)vDataFake;
				}
				OmIds myIdSet;
				void *out = NULL;
				//debug("FIXME", << "in: vData: " << vData << ". " << out << endl;
				myIdSet =
				    setMyColorMap(mCurrentSegmentation, ((SEGMENT_DATA_TYPE *) vData), tile_dims,
						  gotten_id->mTileCoordinate, &out);
				//debug("FIXME", << "out: vData: " << out << endl;
				vData = out;
				//debug("genone", "CALL2 %x\n", vData);
				OmTextureIDUpdate(gotten_id, gotten_id->mTileCoordinate, 0, (tile_dims.x * tile_dims.y),
						  tile_dims.x, tile_dims.y, myIdSet, vData, OMTILE_NEEDCOLORMAP);
			}
			//debug("genone", "ELSE %x\n", vData);
		}
		//debug("genone", "VALID\n");
	} else {
		//debug("genone", "MIP COORD IS INVALID\n");
	}
}

void OmView2d::PreDraw(Vector2i zoomMipVector)
{
	drawComplete = true;
	unsigned int freshness = 0;
	//debug("genone","OmView2d::Draw(zoom lvl %i, scale %i)\n", zoomMipVector.x, zoomMipVector.y);

	//zoomMipVector = OmStateManager::Instance()->GetZoomLevel();

	Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);
	float zoomFactor = (zoomMipVector.y / 10.0);
	float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);

	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
	int mDataDepth = data_coord.z;

	int tileLength;
	switch (mCache->mVolType) {
	case CHANNEL:
		tileLength = OmVolume::GetChannel(mCache->mImageId).GetChunkDimension();
		break;
	case SEGMENTATION:
		tileLength = OmVolume::GetSegmentation(mCache->mImageId).GetChunkDimension();
		freshness = OmCachingThreadedCachingTile::Freshen(false);
		break;
	}

	bool complete = true;
	int xMipChunk;
	int yMipChunk;
	int xval;
	int yval;

	int pl = pow(2, zoomMipVector.x);
	int tl = tileLength * pow(2, zoomMipVector.x);

	if (translateVector.y < 0) {
		//debug("genone", "((abs(translateVector.y) / tl)) * tl * pl  == %i\n", ((abs(translateVector.y) / tl)) * tl * pl);
		yMipChunk = ((abs(translateVector.y) / tl)) * tl * pl;
		yval = (-1 * (abs(translateVector.y) % tl));
	} else {
		yMipChunk = 0;
		yval = translateVector.y;
	}

	for (int y = yval; y < (mTotalViewport.height * (1.0 / zoomFactor));
	     y = y + tileLength, yMipChunk = yMipChunk + tl) {

		if (translateVector.x < 0) {
			//debug ("genone", "((abs(translateVector.x) / tl)) * tl * pl = %i\n", ((abs(translateVector.x) / tl)) * tl * pl);
			xMipChunk = ((abs(translateVector.x) / tl)) * tl * pl;
			xval = (-1 * (abs(translateVector.x) % tl));
		} else {
			xMipChunk = 0;
			xval = translateVector.x;
		}

#if 0
		debug("genone", "tl = %i\n", tl);
		debug("genone", "pl = %i\n", pl);
		//debug("genone", "x = %i\n", x);
		debug("genone", "y = %i\n", y);
		debug("genone", "xval = %i\n", xval);
		debug("genone", "yval = %i\n", yval);
		debug("genone", "translateVector.x = %i\n", translateVector.x);
		debug("genone", "translateVector.y = %i\n", translateVector.y);
		debug("genone", "xMipChunk = %i\n", xMipChunk);
		debug("genone", "yMipChunk = %i\n", yMipChunk);
		debug("genone", "y-thing: = %f\n", (mTotalViewport.height * (1.0 / zoomFactor)));
#endif

		for (int x = xval; x < (mTotalViewport.width * (1.0 / zoomFactor));
		     x = x + tileLength, xMipChunk = xMipChunk + tl) {

			DataCoord this_data_coord = ToDataCoord(xMipChunk, yMipChunk, mDataDepth);;
			SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
			//debug ("genone", "mVolumeType: %i\n", mVolumeType);
			OmTileCoord mTileCoord = OmTileCoord(zoomMipVector.x, this_space_coord, mVolumeType, OmCachingThreadedCachingTile::Freshen(false));
			NormCoord mNormCoord = OmVolume::SpaceToNormCoord(mTileCoord.Coordinate);
			OmMipChunkCoord coord = mCache->mVolume->NormToMipCoord(mNormCoord, mTileCoord.Level);

			shared_ptr < OmMipChunk > my_chunk;
			shared_ptr < OmTextureID > gotten_id = shared_ptr < OmTextureID > ();
			if (mCache->mVolume->ContainsMipChunkCoord(coord)) {
				mCache->mVolume->GetChunk(my_chunk, coord);
				if (my_chunk->IsOpen()) {
					mCache->GetTextureID(gotten_id, mTileCoord);
				} else {
					mCache->GetTextureID(gotten_id, mTileCoord, false);
					//debug("FIXME", << "Not Open" << endl;
				}

				//debug("FIXME", << "tile: " << mTileCoord << " gotten_id:" << gotten_id << endl;

				mTileCount++;
				//if (mTileCount > 38000) return;       // Failsafe hack added by MW.

				if (gotten_id) {
					gotten_id->mVolType = mCache->mVolType;
					if (gotten_id->GetTextureID() == 0) {
						//debug("FIXME", << "no id..." << gotten_id->texture << endl;
						if (NULL == gotten_id->texture && my_chunk->IsOpen()) {
							myBindToTextureID(gotten_id);
							if (gotten_id->texture) {
								safeTexture(gotten_id);
								//debug("FIXME", << "made texture on the fly..." << endl;
							} else {
								gotten_id = shared_ptr < OmTextureID > ();
								mCache->Remove(mTileCoord);
								mCache->GetTextureID(gotten_id, mTileCoord, false);
								complete = false;
							}
						} else if (gotten_id->texture) {
							//debug("FIXME", << "got here...." << endl;
							safeTexture(gotten_id);
						} else {
							gotten_id = shared_ptr < OmTextureID > ();
							mCache->Remove(mTileCoord);
							mCache->GetTextureID(gotten_id, mTileCoord, false);
							complete = false;
						}
					}

					if (gotten_id && gotten_id->GetTextureID() != 0) {
						//debug("FIXME", << "texture is valid! : " << gotten_id->GetTextureID() << endl;
						mTextures.
						    push_back(new
							      Drawable(x, y, tileLength, mTileCoord, zoomFactor,
								       gotten_id));
					} else {
						//debug("FIXME", << "texture is NOT valid! : " << gotten_id->GetTextureID() << endl;
					}

				} else {
					//debug("FIXME", << "not gotton " << mTileCoord << endl;
					//mTextures.push_back (new Drawable (x, y, tileLength, mTileCoord, zoomFactor));
				}
			}
		}
	}
	if (!complete) {
		//debug ("genone", "not complete yet in predraw\n");
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
	} else {
		//debug ("genone", "complete in predraw\n");
	}
}

void OmView2d::Draw(vector < Drawable * >&textures)
{
	for (vector < Drawable * >::iterator it = textures.begin(); textures.end() != it; it++) {
		Drawable *d = *it;
		if (d->mGood) {
			safeDraw(d->zoomFactor, d->x, d->y, d->tileLength, d->gotten_id);
			delete (d);
			if (0 && SEGMENTATION == d->gotten_id->mVolType
			    && OmStateManager::GetSystemMode() == EDIT_SYSTEM_MODE) {
				OmTileCoord coord = d->gotten_id->mTileCoordinate;
				d->gotten_id = shared_ptr < OmTextureID > ();
				d->mGood = false;
				mCache->Remove(coord);
				//debug("FIXME", << "x: " << d->x << " y: " << d->y << endl;
			}
		} else {
			// Find a mip map that can be drawn in for now...TODO.
		}
	}
}

void OmView2d::SendFrameBuffer(QImage * img)
{
	sentTexture = true;
}

/*
 *	Draw cursors.
 */
void OmView2d::DrawCursors()
{

	int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
	int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;

	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	float scaleFactor = (zoomMipVector.y / 10.0);

	// convert mCenter to data coordinates
	SpaceCoord YZslice = SpaceCoord(0, 0, OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW));
	SpaceCoord XZslice = SpaceCoord(0, 0, OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW));
	SpaceCoord XYslice = SpaceCoord(0, 0, OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW));

	int YZslice_depth = SpaceToDataCoord(YZslice).z;
	int XZslice_depth = SpaceToDataCoord(XZslice).z;
	int XYslice_depth = SpaceToDataCoord(XYslice).z;

	//mView3dWidgetManager.CallEnabled( &OmView3dWidget::Draw )
	glDisable(GL_BLEND);

	switch (mViewType) {
	case XY_VIEW:{
			glColor3f(0.2F, 0.9F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i(mTotalViewport.x, (XZslice_depth + heightTranslate) * scaleFactor);	// origin of the line
			glVertex2i(mTotalViewport.x + mTotalViewport.width, (XZslice_depth + heightTranslate) * scaleFactor);	// ending point of the line

			glEnd();

			glColor3f(0.9F, 0.2F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y);	// origin of the line
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y + mTotalViewport.height);	// ending point of the line
			glEnd();

		}
		break;
	case XZ_VIEW:{
			glColor3f(0.2F, 0.2F, 0.9F);
			glBegin(GL_LINES);
			glVertex2i(mTotalViewport.x, (XYslice_depth + heightTranslate) * scaleFactor);	// origin of the line
			glVertex2i(mTotalViewport.x + mTotalViewport.width, (XYslice_depth + heightTranslate) * scaleFactor);	// ending point of the line
			glEnd();

			glColor3f(0.9F, 0.2F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y);	// origin of the line
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y + mTotalViewport.height);	// ending point of the line
			glEnd();
		}
		break;
	case YZ_VIEW:{
			glColor3f(0.2F, 0.2F, 0.9F);
			glBegin(GL_LINES);
			glVertex2i((XYslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y);	// origin of the line
			glVertex2i((XYslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y + mTotalViewport.height);	// ending point of the line
			glEnd();

			glColor3f(0.2F, 0.9F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i(mTotalViewport.x, (XZslice_depth + heightTranslate) * scaleFactor);	// origin of the line
			glVertex2i(mTotalViewport.x + mTotalViewport.width, (XZslice_depth + heightTranslate) * scaleFactor);	// ending point of the line
			glEnd();
		}
		break;
	}
	glColor3f(1.0F, 1.0F, 1.0F);
}

DataBbox OmView2d::SpaceToDataBbox(const SpaceBbox & spacebox)
{
	DataBbox new_data_box = OmVolume::NormToDataBbox(OmVolume::SpaceToNormBbox(spacebox));
	return new_data_box;
}

SpaceBbox OmView2d::DataToSpaceBbox(const DataBbox & databox)
{
	SpaceBbox new_space_box = OmVolume::NormToSpaceBbox(OmVolume::DataToNormBbox(databox));
	return new_space_box;
}

DataCoord OmView2d::SpaceToDataCoord(const SpaceCoord & spacec)
{
	DataCoord new_data_center = OmVolume::NormToDataCoord(OmVolume::SpaceToNormCoord(spacec));
	return new_data_center;
}

SpaceCoord OmView2d::DataToSpaceCoord(const DataCoord & datac)
{
	SpaceCoord new_space_center = OmVolume::NormToSpaceCoord(OmVolume::DataToNormCoord(datac));
	return new_space_center;
}

SpaceCoord OmView2d::ScreenToSpaceCoord(const ScreenCoord & screenc)
{
	
}

ScreenCoord OmView2d::SpaceToScreenCoord(const SpaceCoord & spacec)
{
	
}

ScreenCoord OmView2d::DataToScreenCoord(const DataCoord & datac)
{
	
}

DataCoord OmView2d::ScreenToDataCoord(const ScreenCoord & screenc)
{
	
}

ScreenCoord OmView2d::NormToScreenCoord(const SpaceCoord & normc)
{
	
}

NormCoord OmView2d::ScreenToNormCoord(const ScreenCoord & screenc)
{
	
}

void OmView2d::mouseSetCrosshair(QMouseEvent * event)
{
	Refresh();
	mTextures.clear();
	myUpdate();

	SetDepth(event);

	OmStateManager::SetToolMode(PAN_MODE);
	OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
}

void OmView2d::mousePressEvent(QMouseEvent * event)
{
	switch (OmStateManager::GetSystemMode()) {

		case NAVIGATION_SYSTEM_MODE: {
			clickPoint.x = event->x();
			clickPoint.y = event->y();

			if (event->button() == Qt::LeftButton) {
				const bool crosshair = event->modifiers() & Qt::ControlModifier;
				if( crosshair ){
					mouseSetCrosshair(event);
				} else {
					mouseNavModeLeftButton(event);
				}
			} else if (event->button() == Qt::RightButton) {
				mouseSelectSegment(event);
			}

			cameraMoving = true;
		}
		break;

		case EDIT_SYSTEM_MODE: {
			if (event->button() == Qt::LeftButton) {
				const bool crosshair = event->modifiers() & Qt::ControlModifier;
				if( crosshair ){
					mouseSetCrosshair(event);
				} else {
					mouseEditModeLeftButton(event);
				}
			} else if (event->button() == Qt::RightButton) {
				mouseSelectSegment(event);
			}
		}
		break;
	}
}

void OmView2d::mouseZoomIn()
{
	MouseWheelZoom(15);
}

void OmView2d::mouseZoomOut()
{
	MouseWheelZoom(-15);
}

void OmView2d::mouseZoom(QMouseEvent * event)
{
	const bool zoomOut = event->modifiers() & Qt::ControlModifier;

	if (zoomOut) {
		mouseZoomOut();
	} else {
		mouseZoomIn();
	}
}

void OmView2d::mouseNavModeLeftButton(QMouseEvent * event)
{
	switch (OmStateManager::GetToolMode()) {
	case SELECT_MODE:
		mouseSelectSegment(event);
		break;
	case PAN_MODE:
		// dealt with in mouseMoveEvent()
		break;
	case CROSSHAIR_MODE:
		mouseSetCrosshair(event);
		break;
	case ZOOM_MODE:
		mouseZoom(event);
		OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
		break;
	case ADD_VOXEL_MODE:
		break;
	case SUBTRACT_VOXEL_MODE:
		break;
	case SELECT_VOXEL_MODE:
		break;
	case VOXELIZE_MODE:
		break;
	default:
		break;
	}
}

void OmView2d::mouseEditModeLeftButton(QMouseEvent * event)
{
	debug ("genone", "OmView2d::mouseEditModeLeftButton %i,%i\n", SELECT_MODE, OmStateManager::GetToolMode());
	bool doselection = false;
	bool dosubtract = false;
	mScribbling = true;
	
	SEGMENT_DATA_TYPE data_value;

	switch (OmStateManager::GetToolMode()) {
	case SELECT_MODE:
		// debug ("genone", "Here!\n");
		mouseSelectSegment(event);
		return;
		break;
	case PAN_MODE:
		clickPoint.x = event->x();
		clickPoint.y = event->y();

		// dealt with in mouseMoveEvent()
		return;
		break;
	case CROSSHAIR_MODE:
		mouseSetCrosshair(event);
		return;
		break;
	case ZOOM_MODE:
		mouseZoom(event);
		OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
		return;
		break;
	case ADD_VOXEL_MODE:
		break;
	case SUBTRACT_VOXEL_MODE:
		dosubtract = true;
		break;
	case SELECT_VOXEL_MODE:	
		return;
		break;
	case VOXELIZE_MODE:
		return;
		break;
	default:
		break;
	}

	DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);
	OmId segmentation_id, segment_id;
	if (OmSegmentEditor::GetEditSelection(segmentation_id, segment_id)) {
		//run action
		if (!doselection) {
			if (dosubtract) {
				data_value = NULL_SEGMENT_DATA;
			} else {
				data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
			}
			BrushToolApplyPaint(segmentation_id, globalDataClickPoint, data_value);
		} else {
			PickToolAddToSelection(segmentation_id, globalDataClickPoint);
		}
	} else {
		debug("genone", "No segment_id in edit selection\n");
	}

	lastDataPoint = getMouseClickpointLocalDataCoord(event);;

	myUpdate();

}

void OmView2d::mouseMoveEvent(QMouseEvent * event)
{

	mMousePoint = Vector2f(event->x(), event->y());

	// http://qt.nokia.com/doc/4.5/qt.html#MouseButton-enum
	if (event->buttons() != Qt::LeftButton) {
		// do nothing
	} else {

		switch (OmStateManager::GetSystemMode()) {
		case NAVIGATION_SYSTEM_MODE:
			if (cameraMoving) {
				if (PAN_MODE == OmStateManager::GetToolMode()) {
					mouseMove_NavMode_CamMoving(event);
					OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
				}
			}
			break;
		case EDIT_SYSTEM_MODE:
			EditModeMouseMove(event);
			break;
		}
	}

	myUpdate();
}

bool OmView2d::amInFillMode()
{
	// FIXME
	return false;
}

void OmView2d::EditMode_MouseMove_LeftButton_Scribbling(QMouseEvent * event)
{
	bool doselection = false;

	DataCoord dataClickPoint = getMouseClickpointLocalDataCoord(event);
	DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);

	//store current selection
	OmId segmentation_id, segment_id;
	bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);

	//return if not valid
	if (!valid_edit_selection)
		return;

	//switch on tool mode
	SEGMENT_DATA_TYPE data_value;
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//get value associated to segment id
		data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
		break;

	case SUBTRACT_VOXEL_MODE:
		data_value = NULL_SEGMENT_DATA;
		break;

	case SELECT_VOXEL_MODE:
		doselection = true;
		break;

	default:
		return;
		break;
	}

	if (!doselection) {
		//run action
		BrushToolApplyPaint(segmentation_id, globalDataClickPoint, data_value);
		bresenhamLineDraw(lastDataPoint, dataClickPoint);
	} else {
		// TODO: bug here; ask MattW
		PickToolAddToSelection(segmentation_id, globalDataClickPoint);
	}

	lastDataPoint = dataClickPoint;

	myUpdate();
}

void OmView2d::mouseReleaseEvent(QMouseEvent * event)
{
	switch (OmStateManager::GetSystemMode()) {
	case NAVIGATION_SYSTEM_MODE:
		cameraMoving = false;
		PickToolGetColor(event);
		break;
	case EDIT_SYSTEM_MODE:
		EditModeMouseRelease(event);
		break;
	}
}

void OmView2d::mouseMove_NavMode_CamMoving(QMouseEvent * event)
{
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	Vector2 < int >current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
	Vector2 < int >drag = Vector2 < int >((clickPoint.x - event->x()), clickPoint.y - event->y());

	mDragX += drag.x / (zoomMipVector.y / 10.);
	mDragY += drag.y / (zoomMipVector.y / 10.);
	drag.x = mDragX;
	drag.y = mDragY;
	mDragX = mDragX - drag.x;
	mDragY = mDragY - drag.y;

	OmStateManager::Instance()->SetPanDistance(mViewType,
						   Vector2 < int >(current_pan.x - drag.x, current_pan.y - drag.y));
	SetViewSliceOnPan();
        debug("view2d","current_pan.x: %i   current_pan.y  %i \n",current_pan.x,current_pan.y);
	clickPoint.x = event->x();
	clickPoint.y = event->y();
}

bool OmView2d::doDisplayInformation()
{
	return OmPreferences::GetBoolean(OM_PREF_VIEW2D_SHOW_INFO_BOOL);
}
