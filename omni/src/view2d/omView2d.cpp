#include "omView2d.h"
#include <values.h>

#include "omCachingTile.h"
//#include "omThreadedCachingTile.h"
#include "omTextureID.h"

#include "system/omStateManager.h"
#include "system/omProject.h"
#include "system/omSystemTypes.h"
#include "system/omEventManager.h"
#include "system/omGarbage.h"

#include "segment/omSegment.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#include "segment/actions/segment/omSegmentSelectionAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
//#include "segment/actions/voxel/omVoxelSetAction.h"
#include "segment/actions/voxel/omVoxelSetValueAction.h"
#include "segment/actions/segment/omSegmentMergeAction.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include "segment/omSegmentEditor.h"

#include "common/omGl.h"

#include <QPainter>
#include <QGLFramebufferObject>


#define DEBUG 0

static QGLWidget * sharedwidget=NULL;

/*
 *	Constructs View2d widget.
 */
OmView2d::OmView2d(ViewType viewtype, ObjectType voltype, OmId image_id, OmId second_id, OmId third_id, QWidget *parent) 
: QWidget(parent)
{	
	sharedwidget = (QGLWidget *) OmStateManager::GetPrimaryView3dWidget ();
	DOUT("OmView2d::OmView2d -- " << viewtype);
	
	mViewType = viewtype;
	mVolumeType = voltype;
	mImageId = image_id;
	mSecondId = second_id;
	mThirdId = third_id;
	mAlpha = 1;
	mJoiningSegmentToggled = false;
	mLevelLock = false;
	mFilling = false;
	mBrushToolDiameter = 1;
	mCurrentSegmentId = 0;
	mCurrentSegmentation = 0;
	mEditedSegmentation = 0;
	mElapsed = NULL;
	mBrushToolMaxX = 0;
	mBrushToolMaxY = 0;
	mBrushToolMaxZ = 0;
	mBrushToolMinX = MAXINT;
	mBrushToolMinY = MAXINT;
	mBrushToolMinZ = MAXINT;
	mEmitMovie = false;
	mSlide = 0;
	mDoRefresh = false;
	mDragX = 0;
	mDragY = 0;
	mMIP = false;
	mScribbling = false;
	

	setFocusPolicy(Qt::ClickFocus);		// necessary for receiving keyboard events
	setMouseTracking(true);				// necessary for mouse-centered zooming
	setAutoFillBackground(false);		// necessary for proper QPainter functionality
	
	
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
		OmChannel &current_channel = OmVolume::GetChannel(mImageId);
		mVolume = &current_channel;
		
		//		mCache = new OmCachingTile(mViewType, mVolumeType, image_id, &current_channel);
		OmCachingThreadedCachingTile * fastCache = new OmCachingThreadedCachingTile(mViewType, mVolumeType, image_id, &current_channel, NULL);
		mCache = fastCache->mCache;
		if (fastCache->mDelete) delete fastCache;
		
		mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);
		
		mCache->SetContinuousUpdate(false);
		
		if(mSecondId && mThirdId) {
			OmChannel &second_channel = OmVolume::GetChannel(mSecondId);
			OmChannel &third_channel = OmVolume::GetChannel(mThirdId);
			DOUT("THERE ARE THREE IDS");
			mCache->setSecondMipVolume(CHANNEL, mSecondId, &second_channel);
		}
		
		mRootLevel = current_channel.GetRootMipLevel();
	}
	else {
		OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
		mVolume = &current_seg; 
		mSeg = &current_seg; 

		
		//		mCache = new OmCachingTile(mViewType, mVolumeType, image_id, &current_seg);
		OmCachingThreadedCachingTile * fastCache  = new OmCachingThreadedCachingTile(mViewType, mVolumeType, image_id, &current_seg, NULL);
		mCache = fastCache->mCache;
		if (fastCache->mDelete) delete fastCache;
		
		mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);
		mCache->SetContinuousUpdate(false);
		
		
		if(mSecondId) {
			OmChannel &second_channel = OmVolume::GetChannel(mSecondId);
			mCache->setSecondMipVolume(CHANNEL, mSecondId, &second_channel);
		}
		
		mRootLevel = current_seg.GetRootMipLevel();
	}
	
	//cout << "conversion = " << endl;
	DataCoord test = DataCoord(0,0,0);
	SpaceCoord test2 = DataToSpaceCoord(test);
	//cout << "0 ---> " << test2.z << endl;
	
	test = DataCoord(0,0,1);
	test2 = DataToSpaceCoord(test);
	//cout << "1 ---> " << test2.z << endl;
	
	cameraMoving = false;
	
	drawInformation = OmPreferences::GetBoolean(OM_PREF_VIEW2D_SHOW_INFO_BOOL);
	informationUpdated = false;
	
	//	widthTranslate = 0;
	//	heightTranslate = 0;
	//	
	//	scaleFactor = 1;
	
	mSegmentID = 0;
	mSegmentValue = 0;
	
	//bwarne: cache size now a property of the cache manager, not the volume
	//OmVolume::SetImageCacheMaxSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_VOLUME_CACHE_SIZE_INT) * BYTES_PER_MB);
	//	mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);
	
	
	sentTexture = false;
	
	depthCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT);
	sidesCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT);
	mipCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT);
	
	delete_dirty = false;
	
	iSentIt = false;
} 


OmView2d::~OmView2d()
{
}


#pragma mark 
#pragma mark GL Event Methods
/////////////////////////////////
///////		 GL Event Methods


// The initializeGL() function is called just once, before paintGL() is called.
// More importantly this function is called before "make current" calls.
void OmView2d::initializeGL()
{	
	// IMPORTANT: To cooperate fully with QPainter, we defer matrix stack operations and attribute initialization until
	// the widget needs to be myUpdated.
	DOUT("OmView2d::initializeGL	" << "(" << size().width() << ", " << size().height() << ")");
	DOUT("viewtype = " << mViewType);
	
	mTotalViewport.lowerLeftX = 0;
	mTotalViewport.lowerLeftY = 0;
	mTotalViewport.width = size().width();
	mTotalViewport.height = size().height();
	
	// cout << "mtotalviewport = " << mTotalViewport << endl;
	
	mNearClip = -1;
	mFarClip = 1;
	mZoomLevel = 0;

	// cout << "mTotalViewport = " << mTotalViewport << endl;
	// InitializeCache();
}

void OmView2d::resizeEvent (QResizeEvent * event)
{
	resizeGL (event->size().width(), event->size().height());
	myUpdate ();
}


/*
 *	Window resize event
 */
void OmView2d::resizeGL(int width, int height)
{ 
	DOUT("OmView2d::resizeGL(" << width << ", " << height << ")");
	
	delete pbuffer;
        pbuffer = new QGLPixelBuffer(QSize (width, height), QGLFormat::defaultFormat(), sharedwidget);

	
	mTotalViewport.lowerLeftX = 0;
	mTotalViewport.lowerLeftY = 0;
	mTotalViewport.width = width;
	mTotalViewport.height = height;
	
	SetViewSliceOnPan ();
} 

/*
 *	Paint window event.
 */	
void OmView2d::paintEvent(QPaintEvent *event)
{
	boost::timer t;
        float zoomFactor = OmStateManager::Instance()->GetZoomLevel().y/10.0;
	

	mImage = safePaintEvent (event);


	QPainter painter;
	painter.begin(this);
	painter.drawImage (QPoint (0,0), mImage);

        if (mEmitMovie) {
                QString file = QString ("omniss-%1.png").arg (mSlide);
                if (!mImage.save (file, "png"));
                        cout << "failed to make slide " << mSlide << ": " << file.toStdString() << endl;
                mSlide++;
        }


        // Painting the text at the bottom and around the sides
        if(drawInformation && 0) {

                painter.fillRect(QRect(10, mTotalViewport.height - 70, 250, 40), QColor(0, 0, 0, 200));
                painter.setPen(QColor(255, 255, 255, 200));
                painter.setFont(QFont("Helvetica", 12, QFont::Bold, true));

                int zlevel = OmStateManager::Instance()->GetZoomLevel().x;

                float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
                DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
                int mViewDepth = data_coord.z;

                if(mVolumeType == SEGMENTATION)
                        painter.drawText(QRect(10, mTotalViewport.height - 65, 250, 30), (Qt::AlignLeft | Qt::TextWordWrap),
                                                         QString("Mip Level: %1    Depth: %2 \nSegment ID: %3    Value: %4").arg(zlevel)
                                                         .arg(mViewDepth)
                                                         .arg(mSegmentID)
                                                         .arg(mSegmentValue));
                else
                        painter.drawText(QRect(10, mTotalViewport.height - 65, 250, 30), (Qt::AlignLeft | Qt::TextWordWrap),
                                                         QString("Mip Level: %1    Depth: %2").arg(zlevel).arg(mViewDepth));

                informationUpdated = false;
        }

        QPen the_pen;
        switch(mViewType) {
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

	if (mFilling) {
		painter.drawRoundedRect (QRect (mMousePoint.x, mMousePoint.y, 20, 20), 5, 5);
	} else if (OmStateManager::GetSystemMode()==EDIT_SYSTEM_MODE) {
		painter.drawEllipse (QRectF (mMousePoint.x-0.5*mBrushToolDiameter*zoomFactor, mMousePoint.y-0.5*mBrushToolDiameter*zoomFactor,
						1.0*mBrushToolDiameter*zoomFactor, 1.0*mBrushToolDiameter*zoomFactor));
	}

        if(hasFocus())
                the_pen.setWidth(5);
        painter.drawRect(mTotalViewport.lowerLeftX, mTotalViewport.lowerLeftY, mTotalViewport.width - 1, mTotalViewport.height - 1);

        if((! cameraMoving) && drawComplete && (! sentTexture)) {
                SendFrameBuffer(&mImage);
        }
        else if(! drawComplete)
                sentTexture = false;


	if (1) {
        	QString str = QString ("MIP Level Locked (Press L to unlock.)");
        	if (mLevelLock) painter.drawText (QPoint (0,mTotalViewport.height-80), str);

		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
        	str = QString::number (zoomMipVector.x, 'f', 2) + QString (" Level");
        	painter.drawText (QPoint (0,mTotalViewport.height-60), str);

		float zoomFactor = (zoomMipVector.y / 10.0);
        	str = QString::number (zoomFactor, 'f', 2) + QString (" zoomFactor");
        	painter.drawText (QPoint (0,mTotalViewport.height-40), str);

		float tiles = 10;
        	str = QString::number (mTileCount, 'f', 0) + QString (" tile(s)");
        	painter.drawText (QPoint (0,mTotalViewport.height-20), str);

		if (!mScribbling) {
        		str = QString::number (t.elapsed(), 'f', 4) + QString (" s");
        		painter.drawText (QPoint (0,mTotalViewport.height), str);
		} else {
			if (mElapsed) {
        			str = QString::number (1.0 / mElapsed->elapsed(), 'f', 4) + QString (" fps");
        			painter.drawText (QPoint (0,mTotalViewport.height), str);
				mElapsed = new boost::timer ();
			} else {
				mElapsed = new boost::timer ();
			}
		}
	}

        painter.end();

	SetViewSliceOnPan ();
}

QImage OmView2d::safePaintEvent(QPaintEvent *event)
{ 	
	DOUT("OmView2d::paintEvent -- " << mViewType);
	//	DOUT("mTotalViewport = " << mTotalViewport);

	mTextures.clear ();
	
	mTileCount = 0;
	initializeGL ();
	pbuffer->makeCurrent();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix(); {
	
		glMatrixMode (GL_PROJECTION);
		glViewport(mTotalViewport.lowerLeftX, mTotalViewport.lowerLeftY, mTotalViewport.width, mTotalViewport.height);
		glLoadIdentity ();
	
	
	
		glOrtho(	mTotalViewport.lowerLeftX, 			/* left */
				mTotalViewport.width,	/* right */
				mTotalViewport.height,	/* bottom */
				mTotalViewport.lowerLeftY,			/* top */
				mNearClip, mFarClip );
	
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		glEnable (GL_TEXTURE_2D);
		OmGarbage::safeCleanTextureIds ();
	
		if (CHANNEL == mVolumeType) {
			float alpha = 0;

			glDisable(GL_BLEND);
			DrawFromCache();

			OmChannel &current_channel = OmVolume::GetChannel(mImageId);
 			const set<OmId> objectIDs = current_channel.GetValidFilterIds();
        		set<OmId>::iterator obj_it;

        		for( obj_it=objectIDs.begin(); obj_it != objectIDs.end(); obj_it++ ) {
        			OmFilter &filter = current_channel.GetFilter(*obj_it);

				alpha = filter.GetAlpha();
				glEnable(GL_BLEND);							// enable blending for transparency
				glBlendFunc (GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);
				glBlendColor (1.f, 1.f, 1.f, (1.f-alpha));

				//cout << filter.GetName() << " in alpha: " << alpha << " " << alpha*255 << endl;
				DrawFromFilter(filter);
				glDisable(GL_BLEND);							// enable blending for transparency
			}

		} else
			DrawFromCache();
		DrawCursors();
	} glPopMatrix();

	glDisable (GL_TEXTURE_2D);

	pbuffer->doneCurrent ();
	return pbuffer->toImage();
} 


#pragma mark 
#pragma mark MouseEvent Methods
/////////////////////////////////
///////		 MouseEvent Methods

void OmView2d::mousePressEvent(QMouseEvent *event) {
	

	switch(OmStateManager::GetSystemMode()) {
		case NAVIGATION_SYSTEM_MODE: {
			clickPoint.x = event->x();
			clickPoint.y = event->y();
			
			if (event->button() == Qt::RightButton) {
				NavigationModeMouseDoubleClick (event);
			}

			cameraMoving = true;
		}
			break;
			
		case EDIT_SYSTEM_MODE: {
			DOUT("edit mode mouse pressed")
			EditModeMousePressed(event);
		}
			break;
	}
	
	
	
} 

void OmView2d::Refresh ()
{
	mDoRefresh = true;
}

void OmView2d::PickToolGetColor(QMouseEvent *event) {
	QRgb p;
	int r, g, b, a;

	p = mImage.pixel (event->pos());
	r = qRed(mImage.pixel (event->pos()));
	g = qGreen(mImage.pixel (event->pos()));
	b = qBlue(mImage.pixel (event->pos()));
	a = qAlpha(mImage.pixel (event->pos()));

	//cout  << "p,r,g,b: " << p << ", " << r << ", " << g << ", " << b << endl;
	// FIXME: Need to add to hud.
}

void OmView2d::PickToolAddToSelection (OmId segmentation_id, DataCoord globalDataClickPoint)
{
	OmSegmentation &current_seg = OmVolume::GetSegmentation(segmentation_id);
	int theId = current_seg.GetVoxelSegmentId (globalDataClickPoint);
	if (theId && !current_seg.IsSegmentSelected(theId)) {
        	(new OmSegmentSelectAction(segmentation_id, theId, true))->Run();
                Refresh ();
	} else {
	}

}

DataCoord OmView2d::BrushToolOTGDC(DataCoord off)
{
	DataCoord ret;

        switch(mViewType) {
                case XY_VIEW: {
                        ret.x = off.x;
                        ret.y = off.y;
                        ret.z = off.z;
                }
                        break;
                case XZ_VIEW: {
                        ret.x = off.x;
                        ret.y = off.z;
                        ret.z = off.y;
                }
                        break;
                case YZ_VIEW: {
                        ret.x = off.z;
                        ret.y = off.y;
                        ret.z = off.x;
                }
                        break;
        }
	return ret;
}

static bool BrushTool8[8][8] = {
	{0,  0, 0, 1,  1, 0, 0,  0},

	{0,  0, 1, 1,  1, 1, 0,  0},

	{0,  1, 1, 1,  1, 1, 1,  0},

	{1,  1, 1, 1,  1, 1, 1,  1},

	{1,  1, 1, 1,  1, 1, 1,  1},

	{0,  1, 1, 1,  1, 1, 1,  0},

	{0,  0, 1, 1,  1, 1, 0,  0},

	{0,  0, 0, 1,  1, 0, 0,  0}};

static bool BrushTool32[32][32] = {
	{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
	{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},

	{0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
	{0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
	{0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
	{0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},

	{0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
	{0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},

	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},

	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},

	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
	{0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
	{0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},

	{0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
	{0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
	{0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
	{0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},

	{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0}};


void OmView2d::BrushToolApplyPaint(OmId segid, DataCoord gDC, SEGMENT_DATA_TYPE seg)
{
	DataCoord off;

        switch(mViewType) {
                case XY_VIEW: {
                        off.x = gDC.x;
                        off.y = gDC.y;
                        off.z = gDC.z;
                }
                        break;
                case XZ_VIEW: {
                        off.x = gDC.x;
                        off.y = gDC.z;
                        off.z = gDC.y;
                }
                        break;
                case YZ_VIEW: {
                        off.x = gDC.z;
                        off.y = gDC.y;
                        off.z = gDC.x;
                }
                        break;
        }

	if (1 == mBrushToolDiameter) {
		//(new OmVoxelSetValueAction(segid, gDC, seg))->Run();
		if (segid != 1 && segid != 0) cout << segid << " is the seg id" << endl;
		mEditedSegmentation = segid;
		mCurrentSegmentId = seg;
		mUpdateCoordsSet.insert (gDC);
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
					BrushToolApplyPaint (segid, BrushToolOTGDC(myoff), seg);
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
                                if (sqrt (x*x+y*y) <= savedDia/2.0) {
                                        myoff.x += i - savedDia / 2.0;
                                        myoff.y += j - savedDia / 2.0;
                                        BrushToolApplyPaint (segid, BrushToolOTGDC(myoff), seg);
                                }
                        }
                }
                mBrushToolDiameter = savedDia;
	}
}


void OmView2d::mouseReleaseEvent(QMouseEvent *event) {
	
	switch(OmStateManager::GetSystemMode()) {
		case NAVIGATION_SYSTEM_MODE:
			cameraMoving = false;

			PickToolGetColor (event);

			break;
			
		case EDIT_SYSTEM_MODE: {
			DOUT("edit mode mouse released");
			EditModeMouseRelease(event);
		}
			break;
	}
	
	// signal ViewBoxChangeEvent
}


void OmView2d::mouseMoveEvent(QMouseEvent *event) {
	

	mMousePoint = Vector2f(event->x(), event->y());

	switch(OmStateManager::GetSystemMode()) {
		case NAVIGATION_SYSTEM_MODE: {
			
			if(cameraMoving) {
#if 1
				Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
                        	Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
				Vector2<int> drag = Vector2<int>((clickPoint.x - event->x()), clickPoint.y - event->y());
				//cout << "pan, dragx, dragy, " << current_pan << ", " << drag.x << ", " << drag.y << endl;
				
                                mDragX += drag.x / (zoomMipVector.y/10.);
                                mDragY += drag.y / (zoomMipVector.y/10.);
                                drag.x = mDragX;
                                drag.y = mDragY;
                                mDragX = mDragX - drag.x;
                                mDragY = mDragY - drag.y;

                        	OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x - drag.x, current_pan.y - drag.y));

				SetViewSliceOnPan ();

#if 0
                        	SpaceCoord panSpaceCoord = DataToSpaceCoord(DataCoord(drag.x, drag.y, 0));
                        	if(mViewType == YZ_VIEW) {
                                	Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
                                	OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x - panSpaceCoord.y, slicemin.y - panSpaceCoord.x));

                                	Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
                                	OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x - panSpaceCoord.y, slicemax.y - panSpaceCoord.x));
                        	}
                        	else {


                                	Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
                                	OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x - panSpaceCoord.x, slicemin.y - panSpaceCoord.y));

                                	Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
                                	OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x - panSpaceCoord.x, slicemax.y - panSpaceCoord.y));

					cout << "miny, pscy" << slicemin.y << ", " << panSpaceCoord.y << endl;
					cout << "minx, pscx" << slicemin.x << ", " << panSpaceCoord.x << endl;
					cout << "dragx:dragy == " << drag.x << ":" << drag.y << endl;
                        	}
#endif
				clickPoint.x = event->x();
				clickPoint.y = event->y();
#endif

				// myUpdate();
			}
			
			else if(drawInformation) {
				Vector2f clickPoint = Vector2f(event->x(), event->y());
				
				int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
				int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
				float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
				
				Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
				float scaleFactor = (zoomMipVector.y / 10.0);
				
				Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
				
				DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
				
				int mViewDepth = data_coord.z;
				
				switch(mViewType) {
					case XY_VIEW: {
						DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
						
						
						if (mVolumeType == SEGMENTATION) {
							OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
							mSegmentID = current_seg.GetVoxelSegmentId(dataClickPoint);
							if(mSegmentID)
								mSegmentValue = current_seg.GetValueMappedToSegmentId(mSegmentID);
							else
								mSegmentValue = 0;
							// DOUT("segment ID = " << mSegmentID);
						}
					}
						break;
					case XZ_VIEW: {
						DataCoord dataClickPoint = DataCoord(localClickPoint.x, mViewDepth, localClickPoint.y);
						
						
						if (mVolumeType == SEGMENTATION) {
							OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
							mSegmentID = current_seg.GetVoxelSegmentId(dataClickPoint);
							if(mSegmentID)
								mSegmentValue = current_seg.GetValueMappedToSegmentId(mSegmentID);
							else
								mSegmentValue = 0;
							// DOUT("segment ID = " << mSegmentID);
						}
					}
						break;
					case YZ_VIEW: {
						DataCoord dataClickPoint = DataCoord(mViewDepth, localClickPoint.y, localClickPoint.x);
						
						
						if (mVolumeType == SEGMENTATION) {
							OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
							mSegmentID = current_seg.GetVoxelSegmentId(dataClickPoint);
							if(mSegmentID)
								mSegmentValue = current_seg.GetValueMappedToSegmentId(mSegmentID);
							else
								mSegmentValue = 0;
							// DOUT("segment ID = " << mSegmentID);
						}
					}
				}
				informationUpdated = true;
			}
			
			
		}
			break;
			
		case EDIT_SYSTEM_MODE: {
			if(event->buttons()) {
				EditModeMouseMove(event);
				DOUT("Moving mouse in edit mode");
			}
		}
			break;
	}
	myUpdate();
}


void OmView2d::mouseDoubleClickEvent(QMouseEvent *event) { 
	
	Vector2f clickPoint = Vector2f(event->x(), event->y());
	
	
	int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
	int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
	float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	float scaleFactor = (zoomMipVector.y / 10.0);
	
	Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
	//cout << "localClickPoint double click = " << localClickPoint << endl;
	
	float newDepthX = DataToSpaceCoord(SpaceCoord(0, 0, localClickPoint.x)).z;
	float newDepthY = DataToSpaceCoord(SpaceCoord(0, 0, localClickPoint.y)).z;
	
	if (mVolumeType == SEGMENTATION) {
		switch(OmStateManager::GetSystemMode()) {
			case NAVIGATION_SYSTEM_MODE:
				NavigationModeMouseDoubleClick(event);
				break;
				
			case EDIT_SYSTEM_MODE:
				EditModeMouseDoubleClick(event);
				break;
		}
	} else {
		switch(OmStateManager::GetSystemMode()) {
			case NAVIGATION_SYSTEM_MODE:
				NavigationModeMouseDoubleClick(event);
				break;
				
			case EDIT_SYSTEM_MODE:
				EditModeMouseDoubleClick(event);
				break;
		}
	}
	
	switch(mViewType) {
		case XY_VIEW: {
			// newDepthX = depth in YZ view
			OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, newDepthX);
			
			// newDepthY = depth in XZ view
			OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, newDepthY);
		}
			break;
		case XZ_VIEW: {
			// newDepthX = depth in YZ view
			OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, newDepthX);
			
			// newDepthY = depth in XY view
			OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, newDepthY);
		}
			break;
		case YZ_VIEW: {
			// newDepthX = depth in XY view
			OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, newDepthX);
			
			// newDepthY = depth in XZ view
			OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, newDepthY);
		}
			break;
	}
	
} 


void OmView2d::NavigationModeMouseDoubleClick(QMouseEvent *event) {
	DOUT("Omview2d::NavigationModeMouseDoubleClick");
	//augment if shift pressed
	bool augment_selection = event->modifiers() & Qt::ShiftModifier;
	
	// get ids
	int pick_object_type;
	
	// find segment selected
	//////
	
	Vector2f clickPoint = Vector2f(event->x(), event->y());
	
	
	int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
	int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
	float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	float scaleFactor = (zoomMipVector.y / 10.0);
	
	Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
	
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
	
	int mViewDepth = data_coord.z;
	
	DataCoord dataClickPoint;
	switch(mViewType) {
		case XY_VIEW:
			dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
			break;
		case XZ_VIEW:
			dataClickPoint = DataCoord(localClickPoint.x, mViewDepth, localClickPoint.y);
			break;
		case YZ_VIEW:
			dataClickPoint = DataCoord(mViewDepth, localClickPoint.y, localClickPoint.x);
			break;
	}

	OmId theId;
	OmId seg;
	bool found = false;
	if (mVolumeType == SEGMENTATION) {
		found = true;
		OmSegmentation &current_seg = OmVolume::GetSegmentation (mImageId);
		seg = mImageId;
		theId = current_seg.GetVoxelSegmentId(dataClickPoint);
	} else {
                OmChannel &current_channel = OmVolume::GetChannel(mImageId);
                const set<OmId> objectIDs = current_channel.GetValidFilterIds();
                set<OmId>::iterator obj_it;

                for( obj_it=objectIDs.begin(); obj_it != objectIDs.end(); obj_it++ ) {
                	OmFilter &filter = current_channel.GetFilter(*obj_it);
			if (filter.GetSegmentation ()) {
				found = true;
				seg = filter.GetSegmentation ();
				OmSegmentation &current_seg = OmVolume::GetSegmentation(seg);
				theId = current_seg.GetVoxelSegmentId(dataClickPoint);
				break;
			}
		}
	}
	if (!found) {
		return;
	}

	OmSegmentation &current_seg = OmVolume::GetSegmentation (seg);
	
	// cout << "omSegmentID = " << mSegmentID << endl;
	//cout << "theID = " << theId << endl;
	if(current_seg.IsSegmentValid(theId)) {
		
		//get segment state
		bool new_segment_select_state = !(OmVolume::GetSegmentation( seg ).IsSegmentSelected( theId ));
		
		//if not augmenting slection and selecting segment
		if(!augment_selection && new_segment_select_state) {
			//get current selection
			OmSegmentation &r_segmentation = OmVolume::GetSegmentation(seg);
			//select new segment, deselect current segments
			OmIds select_segment_ids;
			select_segment_ids.insert(theId);
			(new OmSegmentSelectAction(seg, select_segment_ids, r_segmentation.GetSelectedSegmentIds()))->Run();
			
		} else {
			//set state of 
			(new OmSegmentSelectAction(seg, theId, new_segment_select_state))->Run();
		}
		Refresh ();
		mTextures.clear ();
		myUpdate ();
	}
}

#pragma mark 
#pragma mark Edit Mode Methods
/////////////////////////////////
///////		 Edit Mode Methods

void OmView2d::EditModeMousePressed(QMouseEvent *event) {
	// VoxelEditMouse(event, false);
	bool doselection = false;

	if (mFilling) return;

	// START PAINTING
	if (event->button() == Qt::LeftButton) {
		
		Vector2f clickPoint = Vector2f(event->x(), event->y());

		int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
		int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
		float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
		
		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
		float scaleFactor = (zoomMipVector.y / 10.0);
		
		Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
		
		DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
		int mViewDepth = data_coord.z;
		
		DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
		//cout << "mouse pressed: " << dataClickPoint << endl;
		mScribbling = true;
		
		
		// modifiedCoords.clear();

		// okay, dataClickPoint is flat, only valid in XY ortho view.  This is not correct.  Needs to be global voxel coordinate.
		DataCoord globalDataClickPoint;
		switch(mViewType) {
			case XY_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
				//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
			}
				break;
			case XZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
			}
				break;
			case YZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
			}
				break;
		}
		//cout << "global click point: " << globalDataClickPoint << endl;
		
		//store current selection
		OmId segmentation_id, segment_id;
		bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
		
		//return if not valid
		if(!valid_edit_selection) return;
		
		//switch on tool mode
		SEGMENT_DATA_TYPE data_value;
		switch(OmStateManager::GetToolMode()) {
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
		
		//run action
		if (!doselection) BrushToolApplyPaint(segmentation_id, globalDataClickPoint, data_value);
		else PickToolAddToSelection (segmentation_id, globalDataClickPoint);
		
		
		lastDataPoint = dataClickPoint;

		myUpdate ();
	} else if (event->button() == Qt::RightButton) {
		NavigationModeMouseDoubleClick (event);
	}
	
}


void OmView2d::FillToolFill (OmId seg, DataCoord gCP, SEGMENT_DATA_TYPE fc, SEGMENT_DATA_TYPE bc, int depth) {

	DataCoord  off;
	OmId segid = OmVolume::GetSegmentation(seg).GetVoxelSegmentId (gCP);

	if (!segid) return;
	if (depth > 5000) return;
	depth++;

	//cout << gCP << " filling... in " << segid << " with fc of " << fc << "  and bc of " << bc <<  " at " << depth << endl;

	if (segid != fc && segid == bc) {

        	switch(mViewType) {
                	case XY_VIEW: {
                        	off.x = gCP.x;
                        	off.y = gCP.y;
                        	off.z = gCP.z;
                	}
                        	break;
                	case XZ_VIEW: {
                        	off.x = gCP.x;
                        	off.y = gCP.z;
                        	off.z = gCP.y;
                	}
                        	break;
                	case YZ_VIEW: {
                        	off.x = gCP.z;
                        	off.y = gCP.y;
                        	off.z = gCP.x;
                	}
                        	break;
        	}

		(new OmVoxelSetValueAction(seg, gCP, fc))->Run();
		//delete new OmVoxelSetValueAction(seg, gCP, fc);

		
		off.x++;
		FillToolFill (seg, BrushToolOTGDC (off), fc, bc, depth);
		off.y++; 
		off.x--;
		FillToolFill (seg, BrushToolOTGDC (off), fc, bc, depth);
		off.x--;
		off.y--;
		FillToolFill (seg, BrushToolOTGDC (off), fc, bc, depth);
		off.y--;
		off.x++;
		FillToolFill (seg, BrushToolOTGDC (off), fc, bc, depth);
	}
}


void OmView2d::EditModeMouseRelease(QMouseEvent *event) {
	// END PAINTING
	
	bool doselection = false;

	if (event->button() == Qt::LeftButton && mFilling) {
                Vector2f clickPoint = Vector2f(event->x(), event->y());


                int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
                int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
                float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);

                Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
                float scaleFactor = (zoomMipVector.y / 10.0);

                Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);

                DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
                int mViewDepth = data_coord.z;

                DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
                //drawLineTo(QPoint(clickPoint.x, clickPoint.y));
                mScribbling = false;

                // okay, dataClickPoint is flat, only valid in XY ortho view.  This is not correct.  Needs to be global voxel coordinate.
                DataCoord globalDataClickPoint;
                switch(mViewType) {
                        case XY_VIEW: {
                                globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
                                //                                      cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
                        }
                                break;
                        case XZ_VIEW: {
                                globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
                        }
                                break;
                        case YZ_VIEW: {
                                globalDataClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
                        }
                                break;
                }

                //store current selection
                OmId segmentation_id, segment_id;
                bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);

                //return if not valid
                if(!valid_edit_selection) return;

                //switch on tool mode
                SEGMENT_DATA_TYPE data_value;
                switch(OmStateManager::GetToolMode()) {
                        case ADD_VOXEL_MODE:
                                //get value associated to segment id
                                data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
                                break;

                        case SUBTRACT_VOXEL_MODE:
                                data_value = NULL_SEGMENT_DATA;
                                break;

                        default:
                                //assert(false);
				break;
                }


		OmId segid = OmVolume::GetSegmentation(segmentation_id).GetVoxelSegmentId (globalDataClickPoint);
		FillToolFill (segmentation_id, globalDataClickPoint, data_value, segid);
		mFilling = false;		// Set false to avoid accidental overuse.
		Refresh ();
		mTextures.clear ();
		myUpdate();
	}
	else if (event->button() == Qt::LeftButton && mScribbling) {
		Vector2f clickPoint = Vector2f(event->x(), event->y());
		
		
		int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
		int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
		float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
		
		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
		float scaleFactor = (zoomMipVector.y / 10.0);
		
		Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
		
		DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
		int mViewDepth = data_coord.z;
		
		DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
		//drawLineTo(QPoint(clickPoint.x, clickPoint.y));
		mScribbling = false;

		// okay, dataClickPoint is flat, only valid in XY ortho view.  This is not correct.  Needs to be global voxel coordinate.
		DataCoord globalDataClickPoint;
		switch(mViewType) {
			case XY_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
				//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
			}
				break;
			case XZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
			}
				break;
			case YZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
			}
				break;
		}
		//cout << "global click point: " << globalDataClickPoint << endl;
		
		//store current selection
		OmId segmentation_id, segment_id;
		bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
		
		//return if not valid
		if(!valid_edit_selection) return;
		
		//switch on tool mode
		SEGMENT_DATA_TYPE data_value;
		switch(OmStateManager::GetToolMode()) {
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
		
		//run action
		if (!doselection) BrushToolApplyPaint(segmentation_id, globalDataClickPoint, data_value);
		else PickToolAddToSelection (segmentation_id, globalDataClickPoint);
		
		lastDataPoint = dataClickPoint;
		 
		myUpdate();
	}
	
}


void OmView2d::EditModeMouseMove(QMouseEvent *event) {
	// KEEP PAINTING
	DOUT("OmView2d::EditModeMouseMove");


	if ((event->buttons() & Qt::LeftButton) && mScribbling) {
		Vector2f clickPoint = Vector2f(event->x(), event->y());
		bool doselection = false;
		
		
		int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
		int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
		float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
		
		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
		float scaleFactor = (zoomMipVector.y / 10.0);
		
		Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
		
		DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
		int mViewDepth = data_coord.z;
		DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
		
		// okay, dataClickPoint is flat, only valid in XY ortho view.  This is not correct.  Needs to be global voxel coordinate.
		DataCoord globalDataClickPoint;
		switch(mViewType) {
			case XY_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
				//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
			}
				break;
			case XZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
			}
				break;
			case YZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
			}
				break;
		}
		//cout << "global click point: " << globalDataClickPoint << endl;

		
		//store current selection
		OmId segmentation_id, segment_id;
		bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
		
		//return if not valid
		if(!valid_edit_selection) return;
		
		//switch on tool mode
		SEGMENT_DATA_TYPE data_value;
		switch(OmStateManager::GetToolMode()) {
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
		
		if (!doselection) {
			//run action
			BrushToolApplyPaint(segmentation_id, globalDataClickPoint, data_value);
			bresenhamLineDraw(lastDataPoint, dataClickPoint);
		} else PickToolAddToSelection (segmentation_id, globalDataClickPoint);
		
		
		lastDataPoint = dataClickPoint;
		
		myUpdate();
	}
	
}


void OmView2d::EditModeMouseDoubleClick(QMouseEvent *event) {
	//NavigationModeMouseDoubleClick(event);
}

void OmView2d::EditModeKeyPress(QKeyEvent *event) {
	
}


void OmView2d::bresenhamLineDraw(const DataCoord &first, const DataCoord &second) {
	
	//store current selection
	OmId segmentation_id, segment_id;
	bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
	bool doselection = false;
	
	//return if not valid
	if(!valid_edit_selection) return;
	
	//switch on tool mode
	SEGMENT_DATA_TYPE data_value;
	switch(OmStateManager::GetToolMode()) {
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
	
	
	//cout << "first data coord = " << first << endl;
	//cout << "second data coord = " << second << endl;
	//cout << endl;
	
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
	
	if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
	if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
	dy <<= 1;                                                  // dy is now 2*dy
	dx <<= 1;                                                  // dx is now 2*dx
	
	// modifiedCoords.insert(DataCoord(x0, y0, mViewDepth));
	DataCoord myDC = DataCoord(x0, y0, mViewDepth);
	
	// myDC is flat, only valid for XY view.  This is not correct.

	DataCoord globalDC;
	switch(mViewType) {
		case XY_VIEW: {
			globalDC = DataCoord(myDC.x, myDC.y, myDC.z);
			//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
		}
			break;
		case XZ_VIEW: {
			globalDC = DataCoord(myDC.x, myDC.z, myDC.y);
		}
			break;
		case YZ_VIEW: {
			globalDC = DataCoord(myDC.z, myDC.y, myDC.x);
		}
			break;
	}
	//cout << "global click point: " << globalDC << endl;
	
        if (!doselection) BrushToolApplyPaint(segmentation_id, globalDC, data_value);
        else PickToolAddToSelection (segmentation_id, globalDC);

	// cout << "insert: " << DataCoord(x0, y0, 0) << endl;
	
	int lastx = x0 - mBrushToolDiameter/4;
	int lasty = y0 - mBrushToolDiameter/4;
	if (dx > dy) {
		int fraction = dy - (dx >> 1);                         // same as 2*dy - dx
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;                                // same as fraction -= 2*dx
			}
			x0 += stepx;
			fraction += dy;                                    // same as fraction -= 2*dy
			//modifiedCoords.insert(DataCoord(x0, y0, mViewDepth));
			DataCoord myDC = DataCoord(x0, y0, mViewDepth);

			// myDC is flat, only valid for XY view.  This is not correct.
			
			DataCoord globalDC;
			switch(mViewType) {
				case XY_VIEW: {
					globalDC = DataCoord(myDC.x, myDC.y, myDC.z);
					//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
				}
					break;
				case XZ_VIEW: {
					globalDC = DataCoord(myDC.x, myDC.z, myDC.y);
				}
					break;
				case YZ_VIEW: {
					globalDC = DataCoord(myDC.z, myDC.y, myDC.x);
				}
					break;
			}
			//cout << "global click point: " << globalDC << endl;
			
			if (mBrushToolDiameter > 4 && (x1 == x0 || abs (x1 - x0) % (mBrushToolDiameter/4) == 0)) {
				if (!doselection) BrushToolApplyPaint(segmentation_id, globalDC, data_value);
			} else if (doselection || mBrushToolDiameter < 4) {
				if (!doselection) BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				else PickToolAddToSelection (segmentation_id, globalDC);
			}
			// cout << "insert: " << DataCoord(x0, y0, 0) << endl;
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
			switch(mViewType) {
				case XY_VIEW: {
					globalDC = DataCoord(myDC.x, myDC.y, myDC.z);
					//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
				}
					break;
				case XZ_VIEW: {
					globalDC = DataCoord(myDC.x, myDC.z, myDC.y);
				}
					break;
				case YZ_VIEW: {
					globalDC = DataCoord(myDC.z, myDC.y, myDC.x);
				}
					break;
			}
			//cout << "global click point: " << globalDC << endl;
			
			if (mBrushToolDiameter > 4 && (y1==y0 || abs (y1 - y0) % (mBrushToolDiameter/4) == 0)) {
				if (!doselection) BrushToolApplyPaint(segmentation_id, globalDC, data_value);
			} else if (doselection || mBrushToolDiameter < 4) {
				if (!doselection) BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				else PickToolAddToSelection (segmentation_id, globalDC);
			}
			// cout << "insert: " << DataCoord(x0, y0, 0) << endl;
		}
	}
	
}


void OmView2d::GlobalDepthFix (float howMuch)
{
	float depth = OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW)*howMuch;
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, depth));
	int dataDepth = data_coord.z;
	SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, dataDepth));
	OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, space_coord.z, false);

	depth = OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW)*howMuch;
	data_coord = SpaceToDataCoord(SpaceCoord(0, 0, depth));
	dataDepth = data_coord.z;
	space_coord = DataToSpaceCoord(DataCoord(0, 0, dataDepth));
	OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, space_coord.z), false;

	depth = OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW)*howMuch;
	data_coord = SpaceToDataCoord(SpaceCoord(0, 0, depth));
	dataDepth = data_coord.z;
	space_coord = DataToSpaceCoord(DataCoord(0, 0, dataDepth));
	OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, space_coord.z, false);
}


#pragma mark 
#pragma mark KeyEvent Methods
/////////////////////////////////
///////		 KeyEvent Methods

void OmView2d::wheelEvent ( QWheelEvent * event ) {
	DOUT("OmView2d::wheelEvent -- " << mViewType);
	
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;
	
	
	if(numSteps >= 0) {
		// ZOOMING IN
		
		Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
		
		if(!mLevelLock && (current_zoom.y >= 10) && (current_zoom.x > 0)) {
			// need to move to previous mip level
			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x - 1, 6));

			GlobalDepthFix (2.0);
		}
		else 
			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, current_zoom.y + (1 * numSteps)));

		PanOnZoom (current_zoom);
	}
	else {
		// ZOOMING OUT
		
		Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
		
		if(!mLevelLock && (current_zoom.y <= 6) && (current_zoom.x < mRootLevel)) {
			// need to move to next mip level
			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x + 1, 10));

			GlobalDepthFix (0.5);
		}
		
		else if(current_zoom.y > 1) {
			int zoom = current_zoom.y - (1 * (-1 * numSteps));
			if (zoom < 1) zoom = 1;

			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, zoom));
		}

		PanOnZoom (current_zoom);
	}
	
	event->accept();
	
}

void OmView2d::SetViewSliceOnPan () {
        Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);
        Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();

        float pl = pow(2, zoomMipVector.x);
	float scaleFactor = zoomMipVector.y / 10.0;


        Vector2i minCoord = Vector2i(mTotalViewport.lowerLeftX - translateVector.x*pl, mTotalViewport.lowerLeftY - translateVector.y*pl);
        SpaceCoord minSpaceCoord = DataToSpaceCoord(DataCoord(minCoord.x, minCoord.y, 0));
        Vector2i maxCoord = Vector2i(mTotalViewport.width / scaleFactor * pl - translateVector.x*pl, mTotalViewport.height / scaleFactor * pl - translateVector.y*pl);
        SpaceCoord maxSpaceCoord = DataToSpaceCoord(DataCoord(maxCoord.x, maxCoord.y, 0));

        if(mViewType == YZ_VIEW) {
                OmStateManager::SetViewSliceMax(mViewType, Vector2f(maxSpaceCoord.y, maxSpaceCoord.x), false);
                OmStateManager::SetViewSliceMin(mViewType, Vector2f(minSpaceCoord.y, minSpaceCoord.x), false);
        }
        else {
                OmStateManager::SetViewSliceMax(mViewType, Vector2f(maxSpaceCoord.x, maxSpaceCoord.y), false);
                OmStateManager::SetViewSliceMin(mViewType, Vector2f(minSpaceCoord.x, minSpaceCoord.y), false);
        }
}


void OmView2d::PanOnZoom(Vector2<int> current_zoom, bool postEvent) {

	// Update the pan so view stays centered.
	ViewType vts[] = {XY_VIEW, YZ_VIEW, XZ_VIEW};

	for (int i = 0; i < 3; i++) {
		Vector2<int> pro_zoom = OmStateManager::Instance()->GetZoomLevel();
		int widthTranslate = OmStateManager::Instance()->GetPanDistance(vts[i]).x;
		int heightTranslate = OmStateManager::Instance()->GetPanDistance(vts[i]).y;

		if (pro_zoom.x > current_zoom.x) {
			widthTranslate = widthTranslate / 2;
			heightTranslate = heightTranslate / 2;
		} else if (pro_zoom.x < current_zoom.x) {
			widthTranslate = widthTranslate * 2;
			heightTranslate = heightTranslate * 2;
		}

       		OmStateManager::Instance()->SetPanDistance(vts[i], Vector2<int>(widthTranslate, heightTranslate), postEvent);
	}

	SetViewSliceOnPan ();
}

void OmView2d::keyPressEvent(QKeyEvent *event) {
	DOUT("OmView2d::keyPressEvent -- " << mViewType);
	
	
	switch (event->key()) {
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
        		if (1 == mBrushToolDiameter) {
                		mBrushToolDiameter = 2;
        		} else if (2 == mBrushToolDiameter) {
				mBrushToolDiameter = 8;
			} else if (8 == mBrushToolDiameter) {
				mBrushToolDiameter = 32;		
			} else if (32 == mBrushToolDiameter) {
				mBrushToolDiameter = 1;		
			}
			myUpdate ();
		}
			break;
		case Qt::Key_L:
		{
			mLevelLock = !mLevelLock;
			myUpdate ();
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
                		OmChannel &current_channel = OmVolume::GetChannel(mImageId);
                		const set<OmId> objectIDs = current_channel.GetValidFilterIds();
                		set<OmId>::iterator obj_it;

                		for( obj_it=objectIDs.begin(); obj_it != objectIDs.end(); obj_it++ ) {
                        		OmFilter &filter = current_channel.GetFilter(*obj_it);
                        		seg = filter.GetSegmentation ();
                        		if (seg) {
						domerge = true;
                                		break;
                        		}
                		}
        		}

			if (domerge) {
				OmSegmentation &current_seg = OmVolume::GetSegmentation(seg);
				(new OmSegmentMergeAction(seg))->Run();
				current_seg.SetAllSegmentsSelected (false);
				Refresh ();
				mTextures.clear ();
			}

			myUpdate ();
		}
			break;

		case Qt::Key_F:
			// Toggle fill mode.
		{
			mFilling = !mFilling;
			myUpdate ();
		}
			break;
		case Qt::Key_Escape:
		{
			// Attempt to return to a safe camera location.
			mLevelLock = false;
			Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
			current_zoom.x = 0;
			current_zoom.y = 10;
			OmStateManager::Instance()->SetZoomLevel (current_zoom);

        		int widthTranslate = 0;
        		int heightTranslate = 0;

        		OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(widthTranslate, heightTranslate));

			Refresh ();
			mTextures.clear ();
			myUpdate ();

		}
			break;
		case Qt::Key_Minus:
		{
			Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
			
			if(!mLevelLock && (current_zoom.y == 6) && (current_zoom.x < mRootLevel)) {
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x + 1, 10));

				GlobalDepthFix (0.5);
			}
			
			else if(current_zoom.y > 1)
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, current_zoom.y - 1));

			PanOnZoom (current_zoom);

		}
			break;
		case Qt::Key_Equal:
		{
			Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
			
			if(!mLevelLock && (current_zoom.y == 10) && (current_zoom.x > 0)) {
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x - 1, 6));
				GlobalDepthFix (2.0);
			}
			else
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, current_zoom.y + 1));

			PanOnZoom (current_zoom);
		}
			break;
		case Qt::Key_Right:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x + 5, current_pan.y));

			SetViewSliceOnPan ();
			
		}
			break;
		case Qt::Key_Left:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x - 5, current_pan.y));
			
			SetViewSliceOnPan ();
		}
			break;
		case Qt::Key_Up:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x, current_pan.y + 5));
			
			SetViewSliceOnPan ();
		}
			break;
		case Qt::Key_Down:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x, current_pan.y - 5));
			
			SetViewSliceOnPan ();
		}
			break;
		case Qt::Key_W:
		case Qt::Key_PageUp:
			// MOVE UP THE STACK, CLOSER TO VIEWER
		{
			float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
			DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
			
			int mViewDepth = data_coord.z;
			
			SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, mViewDepth + 1));
			
			OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);
			//			OmStateManager::Instance()->SetViewSliceDepth(mViewType, mDepth + 0.005);
			
			
			// myUpdate();
			
		}
			break;
		case Qt::Key_S:
		case Qt::Key_PageDown:
			// MOVE DOWN THE STACK, FARTHER FROM VIEWER
		{
			float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
			DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
			
			int mViewDepth = data_coord.z;
			
			SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, mViewDepth - 1));
			
			OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);
			
		}
			break;

		default:
			QWidget::keyPressEvent(event);
	}
}



#pragma mark 
#pragma mark OmEvent Methods
/////////////////////////////////
///////		 OmEvent Methods

void OmView2d::PreferenceChangeEvent(OmPreferenceEvent *event) {
	
	switch(event->GetPreference()) {
			
		case OM_PREF_VIEW2D_SHOW_INFO_BOOL:
		{
			drawInformation = OmPreferences::GetBoolean(OM_PREF_VIEW2D_SHOW_INFO_BOOL);
			myUpdate();
		}
			break;
		case OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT:
		{
			mCache->SetNewAlpha(OmPreferences::GetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT));
			//DOUT("New alpha = " << OmPreferences::GetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT));
			
			//mCache->ClearCache();
			
			myUpdate();
		}
			break;
		case OM_PREF_VIEW2D_VOLUME_CACHE_SIZE_INT:
		{
			//bwarne: cache size now a property of the cache manager, not the volume
			//OmVolume::SetImageCacheMaxSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_VOLUME_CACHE_SIZE_INT) * BYTES_PER_MB);
		}
			break;
		case OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT:
		{
			//bwarne: cache size now a property of the cache manager, not the volume
			//mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);
			myUpdate();
		}
			break;
		case OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT:
		{
			depthCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT);
		}
			break;
		case OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT:
		{
			sidesCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT);
		}
			break;
		case OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT:
		{
			mipCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT);
		}
			break;
		default:
			return;
	}
}

/*
 * Redraw this widget
 */

void OmView2d::SegmentObjectModificationEvent(OmSegmentEvent *event) {
	//add/remove segment, change state, change selection
	//valid methods: GetModifiedSegmentIds()
	
	DOUT("OmView2d::SegmentObjectModificationEvent");
	
	//	OmIds::iterator itr;
	//	OmIds mod_ids = event->GetModifiedSegmentIds();
	//	for(itr = mod_ids.begin(); itr != mod_ids.end(); itr++) {
	//		DOUT("modified id: " << *itr);
	//	}
	
	if((mVolumeType == SEGMENTATION) && (event->GetModifiedSegmentationId() == mImageId)) {
		// mCache->ClearCache();
		modified_Ids = event->GetModifiedSegmentIds();
		delete_dirty = true;
		
		// GetSelectedVoxels
		
		myUpdate();
	}
}

void OmView2d::SegmentDataModificationEvent(OmSegmentEvent *event) {
	//voxels of a segment have changed
	//valid methods: GetModifiedSegmentIds()
	
	DOUT("OmView2d::SegmentDataModificationEvent");
	
	if((mVolumeType == SEGMENTATION) && (event->GetModifiedSegmentationId() == mImageId)) {
		modified_Ids = event->GetModifiedSegmentIds();
		delete_dirty = true;
		myUpdate();
	}
}

void OmView2d::SegmentEditSelectionChangeEvent(OmSegmentEvent *event) {
	//change segment edit selection
	
	DOUT("OmView2d::SegmentEditSelectionChangeEvent");
	
	if(mVolumeType == SEGMENTATION) {
		//		modified_Ids = event->GetModifiedSegmentIds();
		//		delete_dirty = true;
		//		myUpdate();
		
		// need to myUpdate paintbrush, not anything on the screen 
		
		OmId mentationEditId;
		OmId mentEditId;
		
		if(OmSegmentEditor::GetEditSelection(mentationEditId, mentEditId)) {
			
			if(mentationEditId == mImageId) {
				
				const Vector3<float> &color = OmVolume::GetSegmentation(mentationEditId).GetSegment(mentEditId).GetColor();
				
				//DOUT("SETTING EDIT COLOR");
				editColor = qRgba(color.x * 255, color.y * 255, color.z * 255, 255);
			}
		}
	}
}

void OmView2d::VoxelModificationEvent(OmVoxelEvent *event) {
	//voxels in a segmentation have been modified
	//valid methods: GetSegmentationId(), GetVoxels()
	
	DOUT("OmView2d::VoxelModificationEvent() ---" << mViewType);
	
	// cout << "voxel modification event" << endl;
	DOUT("event segmentation id = " << event->GetSegmentationId());
	DOUT("iSentIt = " << iSentIt);
	//modifiedCoords.clear();
	// cout << "event segmentation id: " << event->GetSegmentationId() << endl;
	if((mVolumeType == SEGMENTATION) && (event->GetSegmentationId() == mImageId)) {
		
		set< DataCoord > modVoxels = event->GetVoxels();
		
		// these voxels are not flat, they are correct for ortho views
		
		set<DataCoord>::iterator itr;
		
		for(itr = modVoxels.begin(); itr != modVoxels.end(); itr++) {
			DOUT("data coord = " << *itr);
			modifiedCoords.insert(*itr);
			
		}

		myUpdate();
	}
}


void OmView2d::SystemModeChangeEvent(OmSystemModeEvent *event) {
	DOUT("OmView2d::SystemModeChangeEvent");
	
	if(mVolumeType == SEGMENTATION) {
		modified_Ids = OmVolume::GetSegmentation(mImageId).GetSelectedSegmentIds();
		delete_dirty = true;
		myUpdate();
	}
	
}




#pragma mark 
#pragma mark Actions
/////////////////////////////////
///////		 Actions


#pragma mark 
#pragma mark ViewEvent Methods
/////////////////////////////////
///////		 ViewEvent Methods

void OmView2d::ViewBoxChangeEvent(OmViewEvent *event) {
	DOUT("OmView2d::ViewBoxChangeEvent -- " << mViewType);
	myUpdate();
}

void OmView2d::ViewPosChangeEvent(OmViewEvent *event) {
	DOUT("OmView2d::ViewPosChangeEvent -- " << mViewType);
	myUpdate();
	
}


void OmView2d::ViewCenterChangeEvent(OmViewEvent *event) {
	DOUT("OmView2d::ViewCenterChangeEvent");
	
	// myUpdate view center
	//	mCenter = OmStateManager::GetViewCenter();
	
	
	// DOUT("mCenter = " << mCenter);
	
	myUpdate();
}

void OmView2d::ViewRedrawEvent(OmViewEvent *event) {
	//DOUT("ViewRedrawEvent");
	
	
	//	if(! drawComplete)
	myUpdate();
}



void OmView2d::myUpdate () {

	if (mEditedSegmentation) {
		(new OmVoxelSetValueAction(mEditedSegmentation, mUpdateCoordsSet, mCurrentSegmentId))->Run();
		mUpdateCoordsSet.clear ();
	}

  	if (!mDoRefresh && mEditedSegmentation) {
		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
        	int tileLength = OmVolume::GetSegmentation(mEditedSegmentation).GetChunkDimension();
        	OmSegmentation &current_seg = OmVolume::GetSegmentation(mEditedSegmentation);

        	OmCachingThreadedCachingTile * fastCache = new OmCachingThreadedCachingTile(mViewType, SEGMENTATION, mEditedSegmentation, &current_seg, NULL);
        	OmThreadedCachingTile *cache = fastCache->mCache;
        	if (fastCache->mDelete) delete fastCache;

#if 0
		cout << "enter" << endl;
		cout << "mBrushToolMinX " << mBrushToolMinX << endl;
		cout << "mBrushToolMinY " << mBrushToolMinY << endl;
		cout << "mBrushToolMinZ " << mBrushToolMinZ << endl;
		cout << "mBrushToolMaxZ " << mBrushToolMaxZ << endl;
		cout << "enter" << endl;
#endif

		mTextures.clear ();

		int xMipChunk, xSave=-1;
		int yMipChunk, ySave=-1;
		int zMipChunk, zSave=-1;
		int step = 1;
		for (int x = mBrushToolMinX; x <= mBrushToolMaxX; x = x + step) {
			for (int y = mBrushToolMinY; y <= mBrushToolMaxY; y = y + step) {
				for (int z = mBrushToolMinZ; z <= mBrushToolMaxZ; z = z + step) {

        				if (mViewType == XY_VIEW) {
						xMipChunk = ((int)(x / tileLength)) * tileLength;
						yMipChunk = ((int)(y / tileLength)) * tileLength;
						zMipChunk = z;
        				}
        				else if (mViewType == XZ_VIEW) {
						xMipChunk = ((int)(x / tileLength)) * tileLength;
						yMipChunk = ((int)(z / tileLength)) * tileLength;
						zMipChunk = y;
        				}
        				else if (mViewType == YZ_VIEW) {
						xMipChunk = ((int)(z / tileLength)) * tileLength;
						yMipChunk = ((int)(y / tileLength)) * tileLength;
						zMipChunk = x;
        				}

					if (xMipChunk != xSave || yMipChunk != ySave || zMipChunk != zSave) {
                       				DataCoord this_data_coord = ToDataCoord (xMipChunk, yMipChunk, zMipChunk);
                       				SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
                       				OmTileCoord tileCoord = OmTileCoord(zoomMipVector.x, this_space_coord);

						cache->Remove (tileCoord);
						//cout << tileCoord << endl;
						//cout << "here " << endl;
					}

					//cout << " x,y,z a:" << x << ", "  << y << ", " << z << endl;

                       			xSave = xMipChunk;
                       			ySave = yMipChunk;
                       			zSave = zMipChunk;
				}
			}
		}
		//cout << "exit" << endl;
	} else  if (mDoRefresh) {
		OmCachingThreadedCachingTile::Refresh ();
		mDoRefresh = false;
	}


        mBrushToolMaxX = 0;
        mBrushToolMaxY = 0;
        mBrushToolMaxZ = 0;
        mBrushToolMinX = MAXINT;
        mBrushToolMinY = MAXINT;
        mBrushToolMinZ = MAXINT;

	update ();
}


#pragma mark 
#pragma mark Draw Methods
/////////////////////////////////
///////		 Draw Methods
void OmView2d::DrawFromFilter(OmFilter &filter)
{
	OmThreadedCachingTile * cache = filter.GetCache (mViewType);
	if (!cache) return;

  	OmThreadedCachingTile *sCache = mCache;
	mCache = cache;
	double alpha = mAlpha;
       	mAlpha = filter.GetAlpha();
	mCurrentSegmentation = filter.GetSegmentation();

	Draw (false);

        mAlpha = alpha;
	mCache = sCache;
}



void OmView2d::DrawFromCache()
{
        if (mVolumeType == CHANNEL) {
                OmChannel &current_channel = OmVolume::GetChannel(mImageId);
                mVolume = &current_channel;

                OmCachingThreadedCachingTile * fastCache = new OmCachingThreadedCachingTile(mViewType, mVolumeType, mImageId, &current_channel, NULL);
                mCache = fastCache->mCache;
                if (fastCache->mDelete) delete fastCache;

                mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);
                mCache->SetContinuousUpdate(false);

		Draw (true);
        }
        else {
		mCurrentSegmentation = mImageId;
                OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
                mVolume = &current_seg;

                OmCachingThreadedCachingTile * fastCache  = new OmCachingThreadedCachingTile(mViewType, mVolumeType, mImageId, &current_seg, NULL);
                mCache = fastCache->mCache;
                if (fastCache->mDelete) delete fastCache;

                mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);
                mCache->SetContinuousUpdate(false);
		Draw (false);
	}

}

extern GGOCTFPointer GGOCTFunction;

void OmView2d::safeTexture(shared_ptr<OmTextureID> gotten_id)
{
	if (OMTILE_NEEDCOLORMAP == gotten_id->flags) {
        	GLuint texture;
        	glGenTextures( 1, &texture );
        	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        	glBindTexture (GL_TEXTURE_2D, texture);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
        	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
        	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
        	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, gotten_id->x, gotten_id->y, 0, GL_RGBA, GL_UNSIGNED_BYTE, gotten_id->texture);
	
        	gotten_id->flags = OMTILE_GOOD;
        	gotten_id->textureID = texture;
		if (gotten_id->texture) free (gotten_id->texture);
		gotten_id->texture = NULL;
	
		//cout << "texture " << gotten_id->GetTextureID() << " was built" << endl;

	} else if (OMTILE_NEEDTEXTUREBUILT == gotten_id->flags) {
        	GLuint texture;
               	glGenTextures( 1, &texture );
               	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
               	glBindTexture (GL_TEXTURE_2D, texture);
               	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
               	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
               	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
               	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
               	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		//cout << "texture " << (int)((unsigned char*)gotten_id->texture)[0] << endl;
		//cout << "x " << gotten_id->x << endl;
		//cout << "y " << gotten_id->y << endl;
		glTexImage2D (GL_TEXTURE_2D, 0, GL_LUMINANCE, gotten_id->x, gotten_id->y, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, ((unsigned char*) gotten_id->texture));

		gotten_id->flags = OMTILE_GOOD;
		gotten_id->textureID = texture;

		if (gotten_id->texture) free (gotten_id->texture);
		gotten_id->texture = NULL;
	}
}

void OmView2d::safeDraw (float zoomFactor, int x, int y, int tileLength, shared_ptr<OmTextureID> gotten_id)
{
        if(mViewType == YZ_VIEW) {
                glMatrixMode(GL_TEXTURE);
                glLoadIdentity();
                glTranslatef(0.5,0.5,0.0);
                glRotatef(-90,0.0,0.0,1.0);
                glTranslatef(-0.5,-0.5,0.0);
                glMatrixMode(GL_MODELVIEW);
        }

	glBindTexture (GL_TEXTURE_2D, gotten_id->GetTextureID());
	glBegin (GL_QUADS);


	if(mViewType == XY_VIEW) {
		glTexCoord2f (0.0f,0.0f); /* lower left corner of image */
		glVertex2f(x * zoomFactor, y * zoomFactor);

		glTexCoord2f (1.0f, 0.0f); /* lower right corner of image */
		glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);

		glTexCoord2f (1.0f, 1.0f); /* upper right corner of image */
		glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);

		glTexCoord2f (0.0f, 1.0f); /* upper left corner of image */
		glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);
		glEnd ();
	} else if(mViewType == XZ_VIEW) {
		glTexCoord2f (0.0f,0.0f); /* lower left corner of image */
		glVertex2f(x * zoomFactor, y * zoomFactor);

		glTexCoord2f (1.0f, 0.0f); /* lower right corner of image */
		glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);

		glTexCoord2f (1.0f, 1.0f); /* upper right corner of image */
		glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);

		glTexCoord2f (0.0f, 1.0f); /* upper left corner of image */
		glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);
		glEnd ();
	} else if(mViewType == YZ_VIEW) {
		glTexCoord2f (0.0f,0.0f); /* lower left corner of image */
		glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);

		glTexCoord2f (1.0f, 0.0f); /* lower right corner of image */
		glVertex2f(x * zoomFactor, y * zoomFactor);

		glTexCoord2f (1.0f, 1.0f); /* upper right corner of image */
		glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);

 		glTexCoord2f (0.0f, 1.0f); /* upper left corner of image */
		glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);
		glEnd ();
	}
}

DataCoord OmView2d::ToDataCoord (int xMipChunk, int yMipChunk, int mDataDepth)
{
        DataCoord this_data_coord;
        switch(mViewType) {
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
        DOUT("OmView2d::Draw() -- " << mViewType);

        Vector2f zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
#if 0
	//cout << "mip: " << mip << endl;
	if (mip) {
        	Vector2f zoom = zoomMipVector;
        	Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);
		int lvl = zoomMipVector.x + 2;
		if (lvl > mRootLevel) lvl = mRootLevel;

		lvl = mRootLevel;

        	for (int i = lvl; i > zoomMipVector.x; i--) {
                	PanOnZoom (zoom, false);
                	zoom.x = i;
                	zoom.y = zoomMipVector.y*(1+i-zoomMipVector.x);

                	OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int> (translateVector.x/(1+i-zoomMipVector.x), translateVector.y/(1+i-zoomMipVector.x)), false);

                	PreDraw(zoom);
                	//cout << "zoom: " << zoom << endl;
        	}
        	OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int> (translateVector.x, translateVector.y), false);

	}
#endif

        PreDraw(zoomMipVector);

        Draw (mTextures);
}

Drawable::Drawable (int x, int y, int tileLength, OmTileCoord tileCoord, float zoomFactor, shared_ptr<OmTextureID> gotten_id)
: x(x), y(y), tileLength(tileLength), tileCoord(tileCoord), zoomFactor(zoomFactor), gotten_id(gotten_id)
{
	mGood = true;
	//cout << x << ", " << y << endl;
	//cout << gotten_id->GetTextureID () << endl;
}
Drawable::Drawable (int x, int y, int tileLength, OmTileCoord tileCoord, float zoomFactor)
: x(x), y(y), tileLength(tileLength), tileCoord(tileCoord), zoomFactor(zoomFactor)
{
	mGood = false;
}

void OmTextureIDUpdate(shared_ptr<OmTextureID>gotten_id, const OmTileCoord tileCoord, const GLuint texID, const int size, int x, int y, const OmIds &containedIds, void *texture, int flags)
{
	//cout << "in OmTextureIDUpdate" << endl;

        gotten_id->mTileCoordinate = tileCoord;
        gotten_id->textureID = texID;
        gotten_id->mem_size = size;
        gotten_id->mIdSet = containedIds;
        gotten_id->flags = flags;
	gotten_id->texture = texture;
	gotten_id->x = x;
	gotten_id->y = y;

        //gotten_id->UpdateSize(size);
}

int OmView2d::GetDepth(const OmTileCoord &key, OmMipVolume *vol) {

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

static int clamp (int c) 
{
	if (c > 255) return 255;
	return c;
}
OmIds OmView2d::setMyColorMap(OmId segid, SEGMENT_DATA_TYPE *imageData, Vector2<int> dims, const OmTileCoord &key, void **rData)
{
        DOUT("OmTile::setMyColorMap(imageData)");

        OmIds found_ids;
	bool entered;

        DataBbox data_bbox = mCache->mVolume->MipCoordToDataBbox(mCache->TileToMipCoord(key), 0);
        int my_depth = GetDepth(key, mCache->mVolume);

        unsigned char *data = new unsigned char[dims.x*dims.y*SEGMENT_DATA_BYTES_PER_SAMPLE];
	//bzero (data, dims.x*dims.y*SEGMENT_DATA_BYTES_PER_SAMPLE);

        int ctr = 0;
        int newctr = 0;
	SEGMENT_DATA_TYPE lastid = 0;

        OmSegmentation &current_seg = OmVolume::GetSegmentation(segid);

	QHash <SEGMENT_DATA_TYPE, QColor> speedTable;
	QColor newcolor;
        OmId id;

        // looping through each value of imageData, which is strictly dims.x * dims.y big, no extra because of cast to SEGMENT_DATA_TYPE
        for (int i = 0 ; i < dims.x * dims.y ; i++) {
                SEGMENT_DATA_TYPE tmpid = (SEGMENT_DATA_TYPE) imageData[i];

                if (tmpid != lastid) {
                        if (!speedTable.contains (tmpid)) {

                                //cout << "gotten segment id mapped to value" << endl;

                		OmId id = current_seg.GetSegmentIdMappedToValue(tmpid);
                                if(id == 0) {
                                        data[ctr] = 0;
                                        data[ctr+1] =  0;
                                        data[ctr+2] =  0;
                                        data[ctr+3] = 255;
                                        newcolor = qRgba(0,0,0,255);
                                } else {

                                        // cout << "asking for color now" << endl;
                                        const Vector3<float> &color = OmVolume::GetSegmentation(segid).GetSegment(id).GetColor();

                                        if(current_seg.IsSegmentSelected(id)) {
						
                                                newcolor = qRgba(clamp (color.x * 255*2.75), clamp(color.y * 255*2.75), clamp(color.z * 255*2.75), 100);
						entered = true;

                                        } else
                                                newcolor = qRgba(color.x * 255, color.y * 255, color.z * 255, 100);

                                        data[ctr] = newcolor.red();
                                        data[ctr+1] = newcolor.green();
                                        data[ctr+2] = newcolor.blue();
                                        data[ctr+3] = 255;

                                }

                                speedTable[tmpid] = newcolor;
                                //cout << " adding to speed table" << endl;

                        } else {
                                //cout << " using speed table" << endl;
                                newcolor = speedTable.value (tmpid);
                                data[ctr] = newcolor.red();
                                data[ctr+1] = newcolor.green();
                                data[ctr+2] = newcolor.blue();
                                data[ctr+3] = 255;
                        }
                } else {
                	data[ctr] = newcolor.red();
                       	data[ctr+1] = newcolor.green();
                       	data[ctr+2] = newcolor.blue();
                       	data[ctr+3] = 255;
                }
                newctr = newctr + 1;
                ctr = ctr+4;
                lastid = tmpid;
        }

     

        // cout << "going to make it the texture now" << endl;
        *rData = data;

        return found_ids;
}


void* OmView2d::GetImageData(const OmTileCoord &key, Vector2<int> &sliceDims, OmMipVolume *vol) {

	//cout << "in OmView2d::GetImageData" << endl;
        shared_ptr<OmMipChunk> my_chunk = vol->GetChunk(mCache->TileToMipCoord(key));
        int depth = GetDepth(key, vol);
        int realDepth = depth % (vol->GetChunkDimension());


        void* void_data = NULL;
        if (mViewType == XY_VIEW) {
                void_data = my_chunk->ExtractDataSlice(VOL_XY_PLANE, realDepth, sliceDims, true);
        }
        else if (mViewType == XZ_VIEW) {
                void_data = my_chunk->ExtractDataSlice(VOL_XZ_PLANE, realDepth, sliceDims, true);
        }
        else if (mViewType == YZ_VIEW) {
                void_data = my_chunk->ExtractDataSlice(VOL_YZ_PLANE, realDepth, sliceDims, true);
        }
        return void_data;
}


void OmView2d::myBindToTextureID(shared_ptr<OmTextureID> gotten_id) {
        //std::cerr << "entering " << __FUNCTION__ << endl;
        int BPS = mCache->mVolume->GetBytesPerSample();
        OmMipChunkCoord coord = mCache->TileToMipCoord(gotten_id->mTileCoordinate);

	//cout << "tile coord: "  << gotten_id->mTileCoordinate << endl;

        if(mCache->mVolume->ContainsMipChunkCoord(coord)) {

                int mcc_x = coord.Coordinate.x;
                int mcc_y = coord.Coordinate.y;
                int mcc_z = coord.Coordinate.z;

                void *vData = NULL;
                if((mcc_x >= 0) && (mcc_y >= 0) && (mcc_z >= 0)) {
                        Vector2<int> tile_dims;
                        vData = GetImageData(gotten_id->mTileCoordinate, tile_dims, mCache->mVolume);

			//cout << "vData: "  << vData << endl;
                        if (mCache->mVolType == CHANNEL) {
                                OmIds myIdSet;
                                OmTextureIDUpdate(gotten_id, gotten_id->mTileCoordinate, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y, myIdSet, vData, OMTILE_NEEDTEXTUREBUILT);
                        }
                        else {
                                if  (1 == BPS) {
                                        cout << "1 bps !!!!!!!!!!!!!!!!!\n" << endl;
                                        uint32_t *vDataFake;
                                        vDataFake = (uint32_t *) malloc ((tile_dims.x * tile_dims.y) * sizeof (SEGMENT_DATA_TYPE));
                                        for (int i = 0; i < (tile_dims.x * tile_dims.y); i++) {
                                                vDataFake[i] = ((unsigned char*)(vData))[i];
                                                vDataFake[i] << 8;
                                                //cout << " "  << (int)((unsigned char*)(vData))[i];
                                        }
                                        cout << endl;
                                        free (vData);
                                        vData = (void *) vDataFake;
                                }
                                OmIds myIdSet;
                                void *out = NULL;
                                //cout << "in: vData: " << vData << ". " << out << endl;
                                myIdSet = setMyColorMap(mCurrentSegmentation, ((SEGMENT_DATA_TYPE *) vData), tile_dims, gotten_id->mTileCoordinate, &out);
                                //cout << "out: vData: " << out << endl;
                                vData = out;
                                OmTextureIDUpdate(gotten_id, gotten_id->mTileCoordinate, 0, (tile_dims.x * tile_dims.y), tile_dims.x, tile_dims.y, myIdSet, vData, OMTILE_NEEDCOLORMAP);
                        }
                }
         	//cout << "VALID " << gotten_id->mTileCoordinate << " vData: " << vData << endl;
        }
	//else
         	//cout << "MIP COORD IS INVALID" << gotten_id->mTileCoordinate << endl;
}



void OmView2d::PreDraw(Vector2i zoomMipVector)
{
	drawComplete = true;
	DOUT("OmView2d::Draw() -- " << mViewType);
	
        //zoomMipVector = OmStateManager::Instance()->GetZoomLevel();

	Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);
	float zoomFactor = (zoomMipVector.y / 10.0);
	float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
	int mDataDepth = data_coord.z;
	
	int tileLength;
	switch(mCache->mVolType) {
		case CHANNEL:
			tileLength = OmVolume::GetChannel(mCache->mImageId).GetChunkDimension();
			break;
		case SEGMENTATION:
			tileLength = OmVolume::GetSegmentation(mCache->mImageId).GetChunkDimension();
			break;
	}

	int xMipChunk;
	int yMipChunk;
	int xval;
	int yval;

	int pl = pow(2, zoomMipVector.x);
	int tl = tileLength * pow(2, zoomMipVector.x);
	
	if(translateVector.y < 0) {
		yMipChunk = ((abs(translateVector.y) / tl)) * tl * pl;
		yval = (-1 * (abs(translateVector.y) % tl));
	} else {
		yMipChunk = 0;
		yval = translateVector.y;
	}

	for (int y = yval; y < (mTotalViewport.height * (1.0 / zoomFactor)) ; y = y + tileLength,yMipChunk = yMipChunk + tl) {
		
		if(translateVector.x < 0) {
			xMipChunk = ((abs(translateVector.x) / tl)) * tl * pl;
			xval = (-1 * (abs(translateVector.x) % tl));
		} else {
			xMipChunk = 0;
			xval = translateVector.x;
		}

#if 0
#define d(x) " "
		cout << d(" tl: ") << tl;
		cout << d(" pl: ") << pl;
		//cout << d(" x: ") << x;
		cout << d(" y: ") << y;
		//cout << d(" xval: ") << xval;
		cout << d(" yval: ") << yval;
		//cout << d(" translateVector.x: ") << translateVector.x;
		cout << d(" translateVector.y: ") << translateVector.y;
		//cout << d(" xMipChunk: ") << xMipChunk << endl;
		cout << d(" yMipChunk: ") << yMipChunk;
		cout << d(" y-thing: ") << (mTotalViewport.height * (1.0 / zoomFactor)) << endl;
#endif

		for (int x = xval; x < (mTotalViewport.width * (1.0/zoomFactor)) ; x = x + tileLength,xMipChunk = xMipChunk + tl) {
                        DataCoord this_data_coord = ToDataCoord (xMipChunk, yMipChunk, mDataDepth);;
                        SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
                        OmTileCoord mTileCoord = OmTileCoord(zoomMipVector.x, this_space_coord);
                        shared_ptr<OmTextureID> gotten_id = mCache->GetTextureID(mTileCoord);

                        //cout << "tile: " << mTileCoord << " gotten_id:" << gotten_id << endl;

                        mTileCount++;
                        //if (mTileCount > 38000) return;       // Failsafe hack added by MW.

                        if(gotten_id) {
                                if(gotten_id->GetTextureID() == 0) {
                                        if (NULL == gotten_id->texture) {
                                                myBindToTextureID (gotten_id);
                                                if (gotten_id->texture) {
                                                        safeTexture (gotten_id);
                                                }
                                        } else {
                                                //cout << "got here...." << endl;
                                                safeTexture (gotten_id);
                                        }
                                }

                                if(gotten_id->GetTextureID() != 0) {
                                        //cout << "texture is valid! : " << gotten_id->GetTextureID() << endl;
                                        mTextures.push_back (new Drawable (x, y, tileLength, mTileCoord, zoomFactor, gotten_id));
                                }
                        } else {
                                //cout << "not gotton " << mTileCoord << endl;
                                mTextures.push_back (new Drawable (x, y, tileLength, mTileCoord, zoomFactor));
                        }
		}
	}
}

void OmView2d::Draw(vector <Drawable*> &textures)
{
	for (vector <Drawable*>::iterator it = textures.begin (); textures.end() != it; it++) {
		Drawable* d = *it;
		if (d->mGood) {
			safeDraw(d->zoomFactor, d->x, d->y, d->tileLength, d->gotten_id);
			if (0 && SEGMENTATION == d->gotten_id->mVolType && OmStateManager::GetSystemMode () == EDIT_SYSTEM_MODE) {
				OmTileCoord coord = d->gotten_id->mTileCoordinate;
				d->gotten_id = shared_ptr<OmTextureID>();
				d->mGood = false;
				mCache->Remove (coord);
				//cout << "x: " << d->x << " y: " << d->y << endl;
			}
		} else {
			// Find a mip map that can be drawn in for now...TODO.
		}
	}
}

void OmView2d::SendFrameBuffer(QImage *img)
{
	sentTexture = true;
#if 0
	DOUT("DRAW IS COMPLETE DRAW IS COMPLETE DRAW IS COMPLETE DRAW IS COMPLETE");
	
	OmStateManager::SetSliceState(SLICE_XY_PLANE, true);
	OmStateManager::SetSliceState(SLICE_XZ_PLANE, true);
	OmStateManager::SetSliceState(SLICE_YZ_PLANE, true);
	
	unsigned char *imgData = img->bits();
	
	OmStateManager::SetViewSliceDataFormat(4, 1);
	
	switch(mViewType) {
		case XY_VIEW: {
			DOUT("setting view slice");
			OmStateManager::SetSliceState(SLICE_XY_PLANE, true);
			OmStateManager::SetViewSlice(SLICE_XY_PLANE, Vector3<int>(size().width(),
																	  size().height(),
																	  1),
										 imgData);
			
		}
			break;
		case XZ_VIEW: {
			OmStateManager::SetSliceState(SLICE_XZ_PLANE, true);
			OmStateManager::SetViewSlice(SLICE_XZ_PLANE, Vector3<int>(size().width(),
																	  size().height(),
																	  1),
										 imgData);
		}
			break;
		case YZ_VIEW: {
			OmStateManager::SetSliceState(SLICE_YZ_PLANE, true);
			OmStateManager::SetViewSlice(SLICE_YZ_PLANE, Vector3<int>(size().width(),
																	  size().height(),
																	  1),
										 imgData);
		}
			break;
	}
	
	//	img->save( "test.bmp", "BMP");
	//	OmSnapshot (mViewType, img);
	
	sentTexture = true;
#endif
}

void OmView2d::InitializeCache()
{
#if 0
	DOUT("OmView2d::InitializeCache() -- " << mViewType);
	// cout << "INITIALIZING CACHE" << endl;
	Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	
	float zoomFactor = (zoomMipVector.y / 10.0);
	
	float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
	int mDataDepth = data_coord.z;
	
	glEnable (GL_TEXTURE_2D); /* enable texture mapping */
	
	// data coord
	int tileLength;
	switch(mVolumeType) {
		case CHANNEL:
			tileLength = OmVolume::GetChannel(mImageId).GetChunkDimension();
			break;
		case SEGMENTATION:
			tileLength = OmVolume::GetSegmentation(mImageId).GetChunkDimension();
			break;
	}
	
	
	int xMipChunk;
	int yMipChunk;
	
	int xval;
	int yval;
	
	// SECOND MIP LEVEL
	int tl = tileLength * pow(2, mipCache);
	// (zoomMipVector.x + 1);
	
	if(translateVector.y < 0) {
		yMipChunk =  (abs(translateVector.y) / tl) * tl;
		yval = (Ymodifier * (abs(translateVector.y) % tl));
	}
	else {
		yMipChunk = 0;
		yval = translateVector.y;
	}
	
	for (int y = 0 ; y < (mTotalViewport.height + 5*tileLength)  ; y = y + tileLength) {
		
		if(translateVector.x < 0) {
			xMipChunk = (abs(translateVector.x) / tl) * tl;
			xval = (Xmodifier * (abs(translateVector.x) % tl));
		}
		else {
			xMipChunk = 0;
			xval = translateVector.x;
		}
		
		for (int x = 0 ; x < (mTotalViewport.width + 5*tileLength)  ; x = x + tileLength) {
			//			cout << "x = " << x << endl;
			//				cout << "d = " << d << endl;
			
			//			DOUT("(x, y) = (" << x << ", " << y << ")");
			//			DOUT("mipchunkX = " << xMipChunk);
			//			DOUT("mipchunkY = " << yMipChunk);
			// cout << "d = " << d << endl;
			DataCoord this_data_coord;
			switch(mViewType) {
				case XY_VIEW: {
					this_data_coord = DataCoord(xMipChunk, yMipChunk, mDataDepth);
					//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
				}
					break;
				case XZ_VIEW: {
					this_data_coord = DataCoord(xMipChunk, mDataDepth, yMipChunk);
				}
					break;
				case YZ_VIEW: {
					this_data_coord = DataCoord(mDataDepth, yMipChunk, xMipChunk);
				}
					break;
			}
			
			
			SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
			
			OmTileCoord mTileCoord = OmTileCoord(1, this_space_coord);	
			
			shared_ptr<OmTextureID> gotten_id = mCache->GetTextureID(mTileCoord);
			
			if(gotten_id) {
				
				
				if(delete_dirty) {
					
					if(gotten_id->GetTextureID() != 0) {
						
						OmIds::iterator itr;
						if(! modified_Ids.empty()) {
							
							for(itr = modified_Ids.begin(); itr != modified_Ids.end(); itr++) {
								
								//DOUT("modified id: " << *itr);
								if(gotten_id->FindId(*itr)) {
									//DOUT("modified id: " << *itr);
									//										cout << "tile coordinate: " << gotten_id->GetCoordinate().Coordinate << endl;
									//										cout << "data tile coordinate: " << SpaceToDataCoord(gotten_id->GetCoordinate().Coordinate) << endl;
									
									DataCoord first_coord = SpaceToDataCoord(gotten_id->GetCoordinate().Coordinate);
									
									//mCache->subFullImageTex(gotten_id, first_coord, tileLength);
									
									
									//										gotten_id.reset();
									//										mCache->Remove(mTileCoord);
									//										
									//										// DELETED THIS TEXTURE ID AND FORCED RELOAD
									//										shared_ptr<OmTextureID> gotten_id = mCache->GetTextureID(mTileCoord);
									
									break;
								}
							}
						}
					}
				}
			}
			
			xMipChunk = xMipChunk + tl;
		}
		// if (gotten_id->GetTextureID() != 0)
		yMipChunk = yMipChunk + tl;
	}
	
	
	
	
	tl = tileLength * pow(2, zoomMipVector.x);
	// (zoomMipVector.x + 1);
	
	if(translateVector.y < 0) {
		yMipChunk =  (abs(translateVector.y) / tl) * tl;
		yval = (Ymodifier * (abs(translateVector.y) % tl));
	}
	else {
		yMipChunk = 0;
		yval = translateVector.y;
	}
	
	for (int y = 0 ; y < (mTotalViewport.height + sidesCache*tileLength)  ; y = y + tileLength) {
		
		if(translateVector.x < 0) {
			xMipChunk = (abs(translateVector.x) / tl) * tl;
			xval = (Xmodifier * (abs(translateVector.x) % tl));
		}
		else {
			xMipChunk = 0;
			xval = translateVector.x;
		}
		
		for (int x = 0 ; x < (mTotalViewport.width + sidesCache*tileLength)  ; x = x + tileLength) {
			//			cout << "x = " << x << endl;
			for(float d = mDataDepth + 1 ; d < mDataDepth + depthCache ; d = d++) {
				//				cout << "d = " << d << endl;
				
				//			DOUT("(x, y) = (" << x << ", " << y << ")");
				//			DOUT("mipchunkX = " << xMipChunk);
				//			DOUT("mipchunkY = " << yMipChunk);
				// cout << "d = " << d << endl;
				DataCoord this_data_coord;
				switch(mViewType) {
					case XY_VIEW: {
						this_data_coord = DataCoord(xMipChunk, yMipChunk, d);
						//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
					}
						break;
					case XZ_VIEW: {
						this_data_coord = DataCoord(xMipChunk, d, yMipChunk);
					}
						break;
					case YZ_VIEW: {
						this_data_coord = DataCoord(d, yMipChunk, xMipChunk);
					}
						break;
				}
				
				
				SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
				
				OmTileCoord mTileCoord = OmTileCoord(zoomMipVector.x, this_space_coord);	
				
				shared_ptr<OmTextureID> gotten_id = mCache->GetTextureID(mTileCoord);
				
				if(gotten_id) {
					
					if(delete_dirty) {
						
						if(gotten_id->GetTextureID() != 0) {
							
							OmIds::iterator itr;
							if(! modified_Ids.empty()) {
								
								for(itr = modified_Ids.begin(); itr != modified_Ids.end(); itr++) {
									
									//DOUT("modified id: " << *itr);
									if(gotten_id->FindId(*itr)) {
										//DOUT("modified id: " << *itr);
										//											cout << "tile coordinate: " << gotten_id->GetCoordinate().Coordinate << endl;
										//											cout << "data tile coordinate: " << SpaceToDataCoord(gotten_id->GetCoordinate().Coordinate) << endl;
										
										//											DataCoord first_coord = SpaceToDataCoord(gotten_id->GetCoordinate().Coordinate);
										//											
										//											mCache->subFullImageTex(gotten_id, first_coord, tileLength);
										
										//DOUT("prior release");
										gotten_id.reset();
										mCache->Remove(mTileCoord);
										//DOUT("after release");
										// DELETED THIS TEXTURE ID AND FORCED RELOAD
										shared_ptr<OmTextureID> gotten_id = mCache->GetTextureID(mTileCoord);
										
										break;
									}
								}
							}
						}
					}
				}
				
			}
			xMipChunk = xMipChunk + tl;
		}
		// if (gotten_id->GetTextureID() != 0)
		yMipChunk = yMipChunk + tl;
	}
	
	
	//glDisable (GL_TEXTURE_2D); /* disable texture mapping */ 
	
	delete_dirty = false;
#endif
}


/*
 *	Draw cursors.
 */
void OmView2d::DrawCursors() {
	
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
	
	switch(mViewType) {
		case XY_VIEW: {
			glColor3f (0.2F, 0.9F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i(mTotalViewport.x, (XZslice_depth + heightTranslate) * scaleFactor); // origin of the line
			glVertex2i(mTotalViewport.x + mTotalViewport.width, (XZslice_depth + heightTranslate) * scaleFactor); // ending point of the line
			
			glEnd( );
			
			glColor3f (0.9F, 0.2F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y); // origin of the line
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y + mTotalViewport.height); // ending point of the line
			glEnd( );
			
		}
			break;
		case XZ_VIEW: {
			glColor3f (0.2F, 0.2F, 0.9F);
			glBegin(GL_LINES);
			glVertex2i(mTotalViewport.x, (XYslice_depth + heightTranslate) * scaleFactor); // origin of the line
			glVertex2i(mTotalViewport.x + mTotalViewport.width, (XYslice_depth + heightTranslate) * scaleFactor); // ending point of the line
			glEnd( );
			
			glColor3f (0.9F, 0.2F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y); // origin of the line
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y + mTotalViewport.height); // ending point of the line
			glEnd( );
		}
			break;
		case YZ_VIEW: {
			glColor3f (0.2F, 0.2F, 0.9F);
			glBegin(GL_LINES);
			glVertex2i((XYslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y); // origin of the line
			glVertex2i((XYslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y + mTotalViewport.height); // ending point of the line
			glEnd( );
			
			glColor3f (0.2F, 0.9F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i(mTotalViewport.x, (XZslice_depth + heightTranslate) * scaleFactor); // origin of the line
			glVertex2i(mTotalViewport.x + mTotalViewport.width, (XZslice_depth + heightTranslate) * scaleFactor); // ending point of the line
			glEnd( );
		}
			break;
	}
	glColor3f(1.0F, 1.0F, 1.0F);
}



DataBbox OmView2d::SpaceToDataBbox(const SpaceBbox &spacebox) {
	DataBbox new_data_box;
	
	switch(mVolumeType) {
		case CHANNEL:
			new_data_box = OmVolume::NormToDataBbox(OmVolume::SpaceToNormBbox(spacebox));
			break;
		case SEGMENTATION:
			new_data_box = OmVolume::NormToDataBbox(OmVolume::SpaceToNormBbox(spacebox));
			break;
			
	}
	
	return new_data_box;
}

SpaceBbox OmView2d::DataToSpaceBbox(const DataBbox &databox) {
	SpaceBbox new_space_box;
	
	switch(mVolumeType) {
		case CHANNEL:
			new_space_box = OmVolume::NormToSpaceBbox(OmVolume::DataToNormBbox(databox));
			break;
		case SEGMENTATION:
			new_space_box = OmVolume::NormToSpaceBbox(OmVolume::DataToNormBbox(databox));
			break;		
	}
	
	return new_space_box;
}


DataCoord OmView2d::SpaceToDataCoord(const SpaceCoord &spacec) {
	
	DataCoord new_data_center;
	
	switch(mVolumeType) {
		case CHANNEL:
			new_data_center = OmVolume::NormToDataCoord(OmVolume::SpaceToNormCoord(spacec));
			break;
		case SEGMENTATION:
			new_data_center = OmVolume::NormToDataCoord(OmVolume::SpaceToNormCoord(spacec));
			break;
			
	}
	
	return new_data_center;
}


SpaceCoord OmView2d::DataToSpaceCoord(const DataCoord &datac) {
	SpaceCoord new_space_center;
	
	switch(mVolumeType) {
		case CHANNEL:
			new_space_center = OmVolume::NormToSpaceCoord(OmVolume::DataToNormCoord(datac));
			break;
		case SEGMENTATION:
			new_space_center = OmVolume::NormToSpaceCoord(OmVolume::DataToNormCoord(datac));
			break;
			
	}
	return new_space_center;
}





