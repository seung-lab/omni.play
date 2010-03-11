#include "omView2d.h"

#include "omCachingTile.h"
#include "omTextureID.h"

#include "system/omStateManager.h"
#include "project/omProject.h"
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
#include "system/omLocalPreferences.h"
#include "segment/omSegmentEditor.h"

#include "common/omGl.h"

#include <QPainter>
#include <QGLFramebufferObject>

#define DEBUG 0

static QGLWidget *sharedwidget = NULL;

/**
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

	setFocusPolicy(Qt::ClickFocus);	// necessary for receiving keyboard events
	setMouseTracking(true);	// necessary for mouse-centered zooming
	setAutoFillBackground(false);	// necessary for proper QPainter functionality

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
	mInitialized = false;

	OmCachingThreadedCachingTile::Refresh();
}

OmView2d::~OmView2d()
{
	OmCachingThreadedCachingTile::Refresh();
}

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
	if (OmLocalPreferences::getStickyCrosshairMode()){
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
	}
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
	if (!mInitialized){
		mInitialized = true;
		if (OmLocalPreferences::getStickyCrosshairMode()){
			OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
		}
	}
	//debug("cross","Paint Event Boolean %i \n",event->erased());
	boost::timer t;
	float zoomFactor = OmStateManager::Instance()->GetZoomLevel().y / 10.0;

	mImage = safePaintEvent(event);
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

		int sliceDepth = GetDepthToDataSlice(mViewType);
		str = QString::number(sliceDepth) + QString(" Slice Depth");
		painter.drawText(QPoint(0, mTotalViewport.height - 40), str);

		if (mTileCountIncomplete) {
			str = QString::number(mTileCountIncomplete, 'f', 0) + QString(" tile(s) incomplete of ");
			str += QString::number(mTileCount, 'f', 0) + QString(" tile(s)");
		} else {
			str = QString::number(mTileCount, 'f', 0) + QString(" tile(s)");
		}
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

	DrawCursors();

	painter.end();

	SetViewSliceOnPan();
}

QImage OmView2d::safePaintEvent(QPaintEvent * event)
{
	//debug("genone","OmView2d::paintEvent -- " << mViewType);
	//      //debug("genone","mTotalViewport = " << mTotalViewport);

	mTextures.clear();

	mTileCount = 0;
	mTileCountIncomplete = 0;
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
		//DrawCursors();
	}
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	pbuffer->doneCurrent();
	return pbuffer->toImage();
}

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


// FIXME: what is going on here? why does it work??
void OmView2d::SetDepth(QMouseEvent * event)
{

	ScreenCoord screenc = ScreenCoord(event->x(),event->y());
	SpaceCoord newDepth = ScreenToSpaceCoord(mViewType,screenc);

	debug ("cross", "click event x,y (%i, %i)\n", clickPoint.x, clickPoint.y);
	debug ("cross", "screenc x,y (%i, %i)\n", screenc.x, screenc.y);
	debug ("cross", "newDepth x,y,z (%f, %f, %f)\n", newDepth.x, newDepth.y,newDepth.z);
	OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, newDepth.z);
	OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, newDepth.y);
	OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, newDepth.x);

	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
}


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
			case XY_VIEW:
				globalDC = DataCoord(myDC.x, myDC.y, myDC.z);
				break;
			case XZ_VIEW:
				globalDC = DataCoord(myDC.x, myDC.z, myDC.y);
				break;
			case YZ_VIEW:
				globalDC = DataCoord(myDC.z, myDC.y, myDC.x);
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


/////////////////////////////////
///////          KeyEvent Methods


void OmView2d::SetViewSliceOnPan()
{
	Vector2f translateVector = GetPanDistance(mViewType);
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();

	float pl = OMPOW(2, zoomMipVector.x);
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
        OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}



void OmView2d::PanAndZoom(Vector2 <int> new_zoom, bool postEvent)
{
	// First We Declare some variables
	ScreenCoord newCrosshairCoord;
	Vector2f oldPan;
	Vector2f newPan;

	// Do the Zoom
	OmStateManager::Instance()->SetZoomLevel(new_zoom);

	if (OmLocalPreferences::getStickyCrosshairMode()){
		SetViewSliceOnPan();
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
		return;
	}

        // Update the pan so view stays centered.
        ViewType vts[] = { XY_VIEW, YZ_VIEW, XZ_VIEW };

        for (int i = 0; i < 3; i++) {
                Vector2 < int >pro_zoom = OmStateManager::Instance()->GetZoomLevel();
                int widthTranslate = OmStateManager::Instance()->GetPanDistance(vts[i]).x;
                int heightTranslate = OmStateManager::Instance()->GetPanDistance(vts[i]).y;

                if (pro_zoom.x > new_zoom.x) {
                        widthTranslate = widthTranslate / 2;
                        heightTranslate = heightTranslate / 2;
                } else if (pro_zoom.x < new_zoom.x) {
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


void quicky()
{
      float x = OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW);
      float y = OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW);
      float z = OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW);
      debug("predraw","Depth (x,y,z): (%f, %f, %f)\n",x,y,z);
}

void OmView2d::MoveUpStackCloserToViewer()
{
	int depth = GetDepthToDataSlice(mViewType);
	SetDataSliceToDepth(mViewType, depth+1);	
   
}

void OmView2d::MoveDownStackFartherFromViewer()
{
	int depth = GetDepthToDataSlice(mViewType);
	SetDataSliceToDepth(mViewType, depth-1);      
}

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

	debug("view2d","OmView2d::SegmentObjectModificationEvent\n");

	Refresh ();
	myUpdate();
}

void OmView2d::SegmentDataModificationEvent(OmSegmentEvent * event)
{
	//voxels of a segment have changed
	//valid methods: GetModifiedSegmentIds()

	debug("view2d","OmView2d::SegmentDataModificationEvent\n");
}

void OmView2d::SegmentEditSelectionChangeEvent(OmSegmentEvent * event)
{
	//change segment edit selection

	debug("view2d","OmView2d::SegmentEditSelectionChangeEvent\n");

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

/////////////////////////////////
///////          Actions

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

		Draw(false);
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
	Vector2f stretch = OmVolume::GetStretchValues(mViewType);

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
		glVertex2f((x + tileLength*stretch.x) * zoomFactor, y * zoomFactor);

		glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
		glVertex2f((x + tileLength*stretch.x) * zoomFactor, (y + tileLength*stretch.y) * zoomFactor);

		glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
		glVertex2f(x * zoomFactor, (y + tileLength*stretch.y) * zoomFactor);
		glEnd();
	} else if (mViewType == XZ_VIEW) {
		glTexCoord2f(0.0f, 0.0f);	/* lower left corner of image */
		glVertex2f(x * zoomFactor, y * zoomFactor);

		glTexCoord2f(1.0f, 0.0f);	/* lower right corner of image */
		glVertex2f((x + tileLength*stretch.x) * zoomFactor, y * zoomFactor);

		glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
		glVertex2f((x + tileLength*stretch.x) * zoomFactor, (y + tileLength*stretch.y) * zoomFactor);

		glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
		glVertex2f(x * zoomFactor, (y + tileLength*stretch.y) * zoomFactor);
		glEnd();
	} else if (mViewType == YZ_VIEW) {
		glTexCoord2f(0.0f, 0.0f);	/* lower left corner of image */
		glVertex2f((x + tileLength*stretch.x) * zoomFactor, y * zoomFactor);

		glTexCoord2f(1.0f, 0.0f);	/* lower right corner of image */
		glVertex2f(x * zoomFactor, y * zoomFactor);

		glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
		glVertex2f(x * zoomFactor, (y + tileLength*stretch.y) * zoomFactor);

		glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
		glVertex2f((x + tileLength*stretch.x) * zoomFactor, (y + tileLength*stretch.y) * zoomFactor);
		glEnd();
	}
}



void OmView2d::Draw(int mip)
{
	drawComplete = true;

	Vector2f zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	if (0) {
		Vector2f zoom = zoomMipVector;
		Vector2f translateVector = GetPanDistance(mViewType);

		int lvl = zoomMipVector.x+1;

		for (int i = mRootLevel; i > lvl; i--) {
			
			zoom.x = i;
			zoom.y = zoomMipVector.y * (1 + i - zoomMipVector.x);
			debug("view2d","OmView2d::Draw(zoom lvl %i, scale %i\n)\n");

			OmStateManager::Instance()->SetPanDistance(mViewType,
								   Vector2f(translateVector.x / (1 + i - zoomMipVector.x),
									 translateVector.y / (1 + i - zoomMipVector.x)),
								   false);

			PreDraw(zoom);
		}
		OmStateManager::Instance()->SetPanDistance(mViewType,
							   Vector2f(translateVector.x, translateVector.y),
							   false);
	}

	PreDraw(zoomMipVector);

	TextureDraw(mTextures);
	mTextures.clear ();
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
	Vector2f mZoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	float factor=OMPOW(2,mZoomMipVector.x);

	int ret;

	switch(mViewType){
	case XY_VIEW:
		ret = (int)(dataCoord.z/factor);
		break;
	case XZ_VIEW:
		ret = (int)(dataCoord.y/factor);
		break;
	case YZ_VIEW:
		ret = (int)(dataCoord.x/factor);
	}

	return ret;
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

	// looping through each value of imageData, which is strictly dims.x * dims.y big, 
	//   no extra because of cast to SEGMENT_DATA_TYPE
	for (int i = 0; i < dims.x * dims.y; i++) {

		SEGMENT_DATA_TYPE tmpid = (SEGMENT_DATA_TYPE) imageData[i];

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

void OmView2d::PreDraw(Vector2f zoomMipVector)
{
	drawComplete = true;
	unsigned int freshness = 0;
	//debug("genone","OmView2d::Draw(zoom lvl %i, scale %i)\n", zoomMipVector.x, zoomMipVector.y);

	//zoomMipVector = OmStateManager::Instance()->GetZoomLevel();

	Vector2f translateVector = GetPanDistance(mViewType);
	float zoomFactor = (zoomMipVector.y / 10.0);
	
	Vector3f depth = Vector3f( 0, 0, 0);
	DataCoord data_coord;
	int mDataDepth;
	switch (mViewType){
	case XY_VIEW:
		depth.z = OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW);
		data_coord = SpaceToDataCoord(depth);
	        mDataDepth = data_coord.z;
		break;
	case XZ_VIEW:
		depth.y = OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW);
		data_coord = SpaceToDataCoord(depth);
	        mDataDepth = data_coord.y;
		break;
	case YZ_VIEW:
		depth.x = OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW);
		data_coord = SpaceToDataCoord(depth);
	        mDataDepth = data_coord.x;
		break;
	}

	
	int tileLength;
	switch (mCache->mVolType) {
	case CHANNEL:
		tileLength = OmVolume::GetChannel(mCache->mImageId).GetChunkDimension();
		break;
	case SEGMENTATION:
		tileLength = OmVolume::GetSegmentation(mCache->mImageId).GetChunkDimension();
		freshness = OmCachingThreadedCachingTile::Freshen(false);
		break;
	case VOLUME:
	case SEGMENT:
	case NOTE:
	case FILTER:
		break;
	}

	
	bool complete = true;
	int xMipChunk;
	int yMipChunk;
	int xval;
	int yval;
	Vector2f stretch = OmVolume::GetStretchValues(mViewType);

	int pl = OMPOW(2, zoomMipVector.x);
	int tl = tileLength * OMPOW(2, zoomMipVector.x);

	if (translateVector.y < 0) {
		//debug("genone", "((abs(translateVector.y) / tl)) * tl * pl  == %i\n", ((abs(translateVector.y) / tl)) * tl * pl);
		yMipChunk = ((abs(translateVector.y) /tl)) * tl * pl;
		yval = (-1 * (abs(translateVector.y) % tl));
	} else {
		yMipChunk = 0;
		yval = translateVector.y;
	}

	for (int y = yval; y < (mTotalViewport.height/zoomFactor/stretch.y);
	     y = y + tileLength, yMipChunk = yMipChunk + tl) {

		if (translateVector.x < 0) {
			//debug ("genone", "((abs(translateVector.x) / tl)) * tl * pl = %i\n", ((abs(translateVector.x) / tl)) * tl * pl);
			xMipChunk = ((abs(translateVector.x) / tl)) * tl * pl;
			xval = (-1 * (abs(translateVector.x) % tl));
		} else {
			xMipChunk = 0;
			xval = translateVector.x;
		}

#if 1
		debug("view2d","mDataDepth = %i\n",mDataDepth);
		debug("view2d", "tl = %i\n", tl);
		debug("view2d", "pl = %i\n", pl);
		//debug("view2d", "x = %i\n", x);
		debug("view2d", "y = %i\n", y);
		debug("view2d", "xval = %i\n", xval);
		debug("view2d", "yval = %i\n", yval);
		debug("view2d", "translateVector.x = %i\n", translateVector.x);
		debug("view2d", "translateVector.y = %i\n", translateVector.y);
		debug("view2d", "xMipChunk = %i\n", xMipChunk);
		debug("view2d", "yMipChunk = %i\n", yMipChunk);
		debug("view2d", "y-thing: = %f\n", (mTotalViewport.height * (1.0 / zoomFactor)));
#endif

		for (int x = xval; x < (mTotalViewport.width * (1.0 / zoomFactor/stretch.x));
		     x = x + tileLength, xMipChunk = xMipChunk + tl) {

			for (int count = -5; count < 6; count++) {
                        	DataCoord this_data_coord = ToDataCoord(xMipChunk, yMipChunk, mDataDepth+count);;
                        	SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
                        	OmTileCoord mTileCoord = OmTileCoord(zoomMipVector.x, this_space_coord, mVolumeType,
										OmCachingThreadedCachingTile::Freshen(false));
                        	NormCoord mNormCoord = OmVolume::SpaceToNormCoord(mTileCoord.Coordinate);
                        	OmMipChunkCoord coord = mCache->mVolume->NormToMipCoord(mNormCoord, mTileCoord.Level);
				shared_ptr < OmTextureID > gotten_id = shared_ptr < OmTextureID > ();
                        	if (mCache->mVolume->ContainsMipChunkCoord(coord)) {
                                	mCache->GetTextureID(gotten_id, mTileCoord, false);
				}
			}

			DataCoord this_data_coord = ToDataCoord(xMipChunk, yMipChunk, mDataDepth);;
			SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
			//debug ("genone", "mVolumeType: %i\n", mVolumeType);
			OmTileCoord mTileCoord = OmTileCoord(zoomMipVector.x, this_space_coord, mVolumeType, OmCachingThreadedCachingTile::Freshen(false));
			NormCoord mNormCoord = OmVolume::SpaceToNormCoord(mTileCoord.Coordinate);
			OmMipChunkCoord coord = mCache->mVolume->NormToMipCoord(mNormCoord, mTileCoord.Level);
			debug ("postdraw", "this_data_coord.(x,y,z): (%i,%i,%i)\n", this_data_coord.x,this_data_coord.y,this_data_coord.z); 
			debug ("postdraw", "this_space_coord.(x,y,z): (%f,%f,%f)\n", this_space_coord.x,this_space_coord.y,this_space_coord.z);
			debug ("postdraw", "coord.(x,y,z): (%f,%f,%f)\n", coord.Coordinate.x,coord.Coordinate.y,coord.Coordinate.z);

			shared_ptr < OmTextureID > gotten_id = shared_ptr < OmTextureID > ();
                        if (mCache->mVolume->ContainsMipChunkCoord(coord)) {
				mCache->GetTextureID(gotten_id, mTileCoord, false);
				mTileCount++;
				if (gotten_id) {
					safeTexture(gotten_id);
					mTextures.push_back(new Drawable(x*stretch.x, y*stretch.y, tileLength, mTileCoord, zoomFactor, gotten_id));
				} else {
					mTileCountIncomplete++;
					complete = false;
				}
			}
			else debug("predrawverbose", "bad coordinates\n");
		}
	}
	if (!complete) {
		//debug ("genone", "not complete yet in predraw\n");
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
	} else {
		//debug ("genone", "complete in predraw\n");
	}
}

/*


*/
void OmView2d::TextureDraw(vector < Drawable * >&textures)
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
	// convert mCenter to data coordinates
	SpaceCoord depth;
	depth.x = OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW);
	depth.y = OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW);
	depth.z = OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW);


	ScreenCoord screenc = SpaceToScreenCoord(mViewType,depth);
	//debug("cross","Crosshair Screen: %i %i \n",screenc.x, screenc.y);
	switch (mViewType) {
	case XY_VIEW:
		painter.setPen(Qt::red);
		painter.drawLine(screenc.x,0,screenc.x,mTotalViewport.height);
		painter.setPen(Qt::green);
		painter.drawLine(0,screenc.y,mTotalViewport.width,screenc.y);
		break;
	case XZ_VIEW:
		painter.setPen(Qt::red);
		painter.drawLine(screenc.x,0,screenc.x,mTotalViewport.height);
		painter.setPen(Qt::blue);
		painter.drawLine(0,screenc.y,mTotalViewport.width,screenc.y);
		break;
	case YZ_VIEW:
		painter.setPen(Qt::blue);
		painter.drawLine(screenc.x,0,screenc.x,mTotalViewport.height);
		painter.setPen(Qt::green);
		painter.drawLine(0,screenc.y,mTotalViewport.width,screenc.y);
		break;
	}


}

bool OmView2d::amInFillMode()
{
	// FIXME
	return false;
}

bool OmView2d::doDisplayInformation()
{
	return OmPreferences::GetBoolean(OM_PREF_VIEW2D_SHOW_INFO_BOOL);
}

