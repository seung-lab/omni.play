#include "gui/widgets/omCursors.h"
#include "project/omProject.h"
#include "segment/actions/omSegmentEditor.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"
#include "system/events/omView3dEvent.h"
#include "system/omLocalPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "view2d/drawable.h"
#include "view2d/omCachingThreadedCachingTile.h"
#include "view2d/omView2d.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "volume/omVoxelSetValueAction.h"

static QGLWidget *sharedwidget = NULL;

/**
 *	Constructs View2d widget.
 */
OmView2d::OmView2d(ViewType viewtype, ObjectType voltype, OmId image_id, QWidget * parent, OmViewGroupState * vgs)
	: OmView2dImpl(parent)

{
	mViewGroupState = vgs;
	sharedwidget = (QGLWidget *) OmStateManager::GetPrimaryView3dWidget();

	mViewType = viewtype;
	mVolumeType = voltype;
	mImageId = image_id;
	mAlpha = 1;
	mJoiningSegmentToggled = false;
	mLevelLock = false;
	mCurrentSegmentId = 0;
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
		mViewGroupState->SetChannel( mImageId );

		OmChannel & current_channel = OmProject::GetChannel(mImageId);
		mVolume = &current_channel;

		OmCachingThreadedCachingTile *fastCache =
			new OmCachingThreadedCachingTile(mViewType, mVolumeType, image_id, &current_channel, NULL, mViewGroupState);
		mCache = fastCache->mCache;
		if (fastCache->mDelete)
			delete fastCache;

		mRootLevel = current_channel.GetRootMipLevel();
	} else {
		mViewGroupState->SetSegmentation( mImageId );

		OmSegmentation & current_seg = OmProject::GetSegmentation(mImageId);
		mVolume = &current_seg;
		mSeg = &current_seg;

		OmCachingThreadedCachingTile *fastCache =
			new OmCachingThreadedCachingTile(mViewType, mVolumeType, image_id, &current_seg, NULL, mViewGroupState);
		mCache = fastCache->mCache;
		if (fastCache->mDelete)
			delete fastCache;

		mRootLevel = current_seg.GetRootMipLevel();
	}

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
	mDrawFromChannel = false;

	OmCachingThreadedCachingTile::Refresh();

#ifdef WIN32
	mGlBlendColorFunction = (GLCOLOR) wglGetProcAddress("glBlendColor");
#endif

	resetWindowState();
	OmCursors::setToolCursor(this);
}

OmView2d::~OmView2d()
{
	OmCachingThreadedCachingTile::Refresh();
}

/////////////////////////////////
///////          GL Event Methods


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
void OmView2d::paintEvent(QPaintEvent *)
{
	if (!mInitialized){
		mInitialized = true;
		if (OmLocalPreferences::getStickyCrosshairMode()){
			OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
		}
	}

	QTime t;
	t.start();
	float zoomFactor = mViewGroupState->GetZoomLevel().y / 10.0;

	mImage = safePaintEvent();
	painter.begin(this);
	painter.drawImage(QPoint(0, 0), mImage);

	if (mEmitMovie) {
		QString file = QString("omniss-%1.png").arg(mSlide);
		if (!mImage.save(file, "png")){
			printf("could not save file %s\n", qPrintable(file));
		}
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

	const bool inEditMode = OmStateManager::GetToolMode() == ADD_VOXEL_MODE ||
		OmStateManager::GetToolMode() == SUBTRACT_VOXEL_MODE ||
		OmStateManager::GetToolMode() == FILL_MODE;

	if (amInFillMode()) {
		painter.drawRoundedRect(QRect(mMousePoint.x, mMousePoint.y, 20, 20), 5, 5);
	} else if (inEditMode){
		const double offset = 0.5 * mViewGroupState->getView2DBrushToolDiameter() * zoomFactor;
		const double width = 1.0 * mViewGroupState->getView2DBrushToolDiameter() * zoomFactor;
		painter.drawEllipse(QRectF
				    (mMousePoint.x - offset,
				     mMousePoint.y - offset,
				     width,
				     width ));
	}

	if (hasFocus()){
		the_pen.setWidth(5);
	}

	painter.drawRect(mTotalViewport.lowerLeftX,
			 mTotalViewport.lowerLeftY,
			 mTotalViewport.width - 1,
			 mTotalViewport.height - 1);

	if ((!cameraMoving) && drawComplete && (!sentTexture)) {
		sentTexture = true;
	} else if (!drawComplete) {
		sentTexture = false;
	}

	if (doDisplayInformation()) {
		QString elapsedTime = QString::number(t.elapsed());
		displayInformation( elapsedTime );
	}

	DrawCursors();

	painter.end();

	SetViewSliceOnPan();
}

void OmView2d::displayInformation( QString & elapsedTime )
{
	const int xoffset = 10;

	QString str = QString("MIP Level Locked (Press L to unlock.)");
	if (mLevelLock) {
		painter.drawText(QPoint(xoffset, mTotalViewport.height - 65), str);
	}

	Vector2i zoomMipVector = mViewGroupState->GetZoomLevel();
	str = QString::number(zoomMipVector.x, 'f', 2) + QString(" Level");
	painter.drawText(QPoint(xoffset, mTotalViewport.height - 55), str);

	float zoomFactor = (zoomMipVector.y / 10.0);
	str = QString::number(zoomFactor, 'f', 2) + QString(" zoomFactor");
	painter.drawText(QPoint(xoffset, mTotalViewport.height - 45), str);

	int sliceDepth = GetDepthToDataSlice(mViewType);
	str = QString::number(sliceDepth) + QString(" Slice Depth");
	painter.drawText(QPoint(xoffset, mTotalViewport.height - 35), str);

	if (mTileCountIncomplete) {
		str = QString::number(mTileCountIncomplete, 'f', 0) + QString(" tile(s) incomplete of ");
		str += QString::number(mTileCount, 'f', 0) + QString(" tile(s)");
	} else {
		str = QString::number(mTileCount, 'f', 0) + QString(" tile(s)");
	}

	painter.drawText(QPoint(xoffset, mTotalViewport.height - 25), str);

	if (!mScribbling) {
		str = elapsedTime + QString(" ms");
		painter.drawText(QPoint(xoffset, mTotalViewport.height - 15), str);
		if (mElapsed) {
			str = QString::number(1.0 / mElapsed->elapsed(), 'f', 4) + QString(" fps");
			painter.drawText(QPoint(xoffset, mTotalViewport.height - 5), str);
			mElapsed->restart();
		} else {
			if (mElapsed != NULL) delete mElapsed;
			mElapsed = new QTime();
			mElapsed->start();
		}
	} else {
		if (mElapsed) {
			str = QString::number(1.0 / mElapsed->elapsed(), 'f', 4) + QString(" fps");
			painter.drawText(QPoint(xoffset, mTotalViewport.height - 5), str);
			mElapsed->restart();
		} else {
			if (mElapsed != NULL) delete mElapsed;
			mElapsed = new QTime();
			mElapsed->start();
		}
	}
}

/////////////////////////////////
///////          MouseEvent Methods

void OmView2d::Refresh()
{
	mDoRefresh = true;
}

void OmView2d::PickToolAddToSelection(const OmId segmentation_id, DataCoord globalDataClickPoint)
{
	OmSegmentation & current_seg = OmProject::GetSegmentation(segmentation_id);
        const OmSegID segID = current_seg.GetVoxelSegmentId(globalDataClickPoint);
        if (segID ) {

               OmSegmentSelector sel(segmentation_id, this, "view2dpick" );
               sel.augmentSelectedSet( segID, true );
               sel.sendEvent();

               //Refresh();
        }
}

void OmView2d::PickToolAddToSelection( OmSegmentSelector & sel,
				       OmSegmentation & current_seg,
				       DataCoord globalDataClickPoint)
{
	const OmSegID segID = current_seg.GetVoxelSegmentId(globalDataClickPoint);
	if (segID ) {
		sel.augmentSelectedSet( segID, true );
	}
}

DataCoord OmView2d::BrushToolOTGDC(DataCoord off)
{
	DataCoord ret;

	switch (mViewType) {
	case XY_VIEW:
		ret.x = off.x;
		ret.y = off.y;
		ret.z = off.z;
		break;
	case XZ_VIEW:
		ret.x = off.x;
		ret.y = off.z;
		ret.z = off.y;
		break;
	case YZ_VIEW:
		ret.x = off.z;
		ret.y = off.y;
		ret.z = off.x;
		break;
	}

	return ret;
}

void OmView2d::BrushToolApplyPaint(OmId segid, DataCoord gDC, OmSegID seg)
{
	DataCoord off;

	switch (mViewType) {
	case XY_VIEW:
		off.x = gDC.x;
		off.y = gDC.y;
		off.z = gDC.z;
		break;
	case XZ_VIEW:
		off.x = gDC.x;
		off.y = gDC.z;
		off.z = gDC.y;
		break;
	case YZ_VIEW:
		off.x = gDC.z;
		off.y = gDC.y;
		off.z = gDC.x;
		break;
	}

	if (1 == mViewGroupState->getView2DBrushToolDiameter() ) {
		//debug("brush", "%i,%i,%i\n", DEBUGV3(gDC));
		//(new OmVoxelSetValueAction(segid, gDC, seg))->Run();
		if (segid != 1 && segid != 0) {
			//debug("FIXME", << segid << " is the seg id" << endl;
		}
		mEditedSegmentation = segid;
		mCurrentSegmentId = seg;
		mUpdateCoordsSet.insert(gDC);
		Refresh();
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
	} else {
		const int savedDia = mViewGroupState->getView2DBrushToolDiameter();
		mViewGroupState->setView2DBrushToolDiameter(1);
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
		mViewGroupState->setView2DBrushToolDiameter(savedDia);
	}
}

void OmView2d::SetDepth(QMouseEvent * event)
{
	ScreenCoord screenc = ScreenCoord(event->x(),event->y());
	SpaceCoord newDepth = ScreenToSpaceCoord(screenc);

	debug ("cross", "click event x,y (%i, %i)\n", clickPoint.x, clickPoint.y);
	debug ("cross", "screenc x,y (%i, %i)\n", screenc.x, screenc.y);
	debug ("cross", "newDepth x,y,z (%f, %f, %f)\n", newDepth.x, newDepth.y,newDepth.z);
	mViewGroupState->SetViewSliceDepth(XY_VIEW, newDepth.z);
	mViewGroupState->SetViewSliceDepth(XZ_VIEW, newDepth.y);
	mViewGroupState->SetViewSliceDepth(YZ_VIEW, newDepth.x);

	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
}


/////////////////////////////////
///////          Edit Mode Methods

void OmView2d::FillToolFill(OmId seg, DataCoord gCP, OmSegID fc, OmSegID bc, int depth)
{

	DataCoord off;
	OmSegmentation & current_seg = OmProject::GetSegmentation(seg);
	OmId segid = OmProject::GetSegmentation(seg).GetVoxelSegmentId(gCP);


	if (!segid) {
		return;
	}
	debug("fill", "OmView2d::FillToolFill, segid, fc, bc, depth %i, %i, %i, %i\n", segid, fc, bc, depth);
	segid = current_seg.GetSegmentCache()->findRoot(current_seg.GetSegmentCache()->GetSegment(segid))->getValue();

	if (depth > 5000)
		return;
	depth++;

	//debug("FIXME", << gCP << " filling... in " << segid << " with fc of " << fc << "  and bc of " << bc <<  " at " << depth << endl;


	if (segid == bc && segid != fc) {

		switch (mViewType) {
		case XY_VIEW:
			off.x = gCP.x;
			off.y = gCP.y;
			off.z = gCP.z;
			break;
		case XZ_VIEW:
			off.x = gCP.x;
			off.y = gCP.z;
			off.z = gCP.y;
			break;
		case YZ_VIEW:
			off.x = gCP.z;
			off.y = gCP.y;
			off.z = gCP.x;
			break;
		}

		//debug("fill", "OmView2d::FillToolFill, off: %i, %i, %i __ %i, %i, %i __ %i, %i, %i\n",
		//	DEBUGV3(off), DEBUGV3(BrushToolOTGDC(off)), DEBUGV3(gCP));

		off.x++;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
		off.y++;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
		off.x--;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
		off.x--;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
		off.y--;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
		off.y--;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
		off.x++;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
		off.x++;
		FillToolFill(seg, BrushToolOTGDC(off), fc, bc, depth);
	} else {
		debug("fill", "not entering %i, %i, segid:%i != fc:%i, bc=%i\n", segid == bc, segid != fc, segid, fc, bc);
	}
}

void myBreak(){}
void checkDC (string /*s*/, DataCoord /*dc*/)
{
#if 0
	if(dc.x == dc.y) {
		debug("brush", "%s: xy: %i = %i\n", s.c_str(), dc.x, dc.y);
		myBreak();
	}
	if(dc.x == dc.z) {
		debug("brush", "%s: xz: %i = %i\n", s.c_str(), dc.x, dc.z);
		myBreak();
	}
	if(dc.y == dc.z) {
		debug("brush", "%s: yz: %i = %i\n", s.c_str(), dc.y, dc.z);
		myBreak();
	}
#endif
}

void OmView2d::bresenhamLineDraw(const DataCoord & first, const DataCoord & second, bool doselection)
{
	//store current selection
	SegmentDataWrapper sdw = OmSegmentEditor::GetEditSelection();

	//return if not valid
	if (!sdw.isValid())
		return;

	const OmId segmentation_id = sdw.getSegmentationID();

	//switch on tool mode
	OmSegID data_value = 0;
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//get value associated to segment id
		data_value = sdw.getID();
		break;

	case SUBTRACT_VOXEL_MODE:
		data_value = NULL_SEGMENT_VALUE;
		break;

	case SELECT_VOXEL_MODE:
		doselection = true;
		break;

	default:
		//assert(false);
		break;
	}

	float mDepth = mViewGroupState->GetViewSliceDepth(mViewType);
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
	int mViewDepth = data_coord.z;
        DataCoord globalDC;
	int y1, y0, x1, x0;

        switch (mViewType) {
        case XY_VIEW:
		y1 = second.y;
		y0 = first.y;
		x1 = second.x;
		x0 = first.x;
                break;
        case XZ_VIEW:
		y1 = second.z;
		y0 = first.z;
		x1 = second.x;
		x0 = first.x;
                break;
        case YZ_VIEW:
		y1 = second.y;
		y0 = first.y;
		x1 = second.z;
		x0 = first.z;
                break;
        }

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

	debug("brush", "coords: %i,%i,%i\n", x0, y0, mViewDepth);
	debug("brush", "mDepth = %f\n", mDepth);

	OmSegmentSelector sel(segmentation_id, this, "view2d_selector" );
	OmSegmentation & current_seg = OmProject::GetSegmentation(segmentation_id);

	if(!doselection) {
		//BrushToolApplyPaint(segmentation_id, first, data_value);
	} else {
		PickToolAddToSelection(sel, current_seg, first);
	}

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

			DataCoord globalDC;
			switch (mViewType) {
			case XY_VIEW:
				globalDC = DataCoord(x0, y0, second.z);
				break;
			case XZ_VIEW:
				globalDC = DataCoord(x0, second.y, y0);
				break;
			case YZ_VIEW:
				globalDC = DataCoord(second.x, y0, x0);
				break;
			}

			checkDC("3", globalDC);

			if (mViewGroupState->getView2DBrushToolDiameter() > 4 && (x1 == x0 || abs(x1 - x0) % (mViewGroupState->getView2DBrushToolDiameter() / 4) == 0)) {
				if (!doselection){
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				} else {
					PickToolAddToSelection(sel, current_seg, globalDC);
				}
			} else if (doselection || mViewGroupState->getView2DBrushToolDiameter() < 4) {
				if (!doselection) {
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				} else {
					PickToolAddToSelection(sel, current_seg, globalDC);
				}
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

                        DataCoord globalDC;
                        switch (mViewType) {
                        case XY_VIEW:
                                globalDC = DataCoord(x0, y0, second.z);
                                break;
                        case XZ_VIEW:
                                globalDC = DataCoord(x0, second.y, y0);
                                break;
                        case YZ_VIEW:
                                globalDC = DataCoord(second.x, y0, x0);
                                break;
                        }

			checkDC("4", globalDC);

			if (mViewGroupState->getView2DBrushToolDiameter() > 4 && (y1 == y0 || abs(y1 - y0) % (mViewGroupState->getView2DBrushToolDiameter() / 4) == 0)) {
				if (!doselection) {
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				} else {
					PickToolAddToSelection(sel, current_seg, globalDC);
				}
			} else if (doselection || mViewGroupState->getView2DBrushToolDiameter() < 4) {
				if (!doselection){
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				} else {
					PickToolAddToSelection(sel, current_seg, globalDC);
				}
			}
		}
	}

	if (doselection) {
		if(sel.sendEvent()){
			//Refresh();
		}
	}
}


/////////////////////////////////
///////          KeyEvent Methods


void OmView2d::SetViewSliceOnPan()
{
	Vector2f translateVector = GetPanDistance(mViewType);
	Vector2i zoomMipVector = mViewGroupState->GetZoomLevel();

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
		mViewGroupState->SetViewSliceMax(mViewType, Vector2f(maxSpaceCoord.y, maxSpaceCoord.x), false);
		mViewGroupState->SetViewSliceMin(mViewType, Vector2f(minSpaceCoord.y, minSpaceCoord.x), false);
	} else {
		mViewGroupState->SetViewSliceMax(mViewType, Vector2f(maxSpaceCoord.x, maxSpaceCoord.y), false);
		mViewGroupState->SetViewSliceMin(mViewType, Vector2f(minSpaceCoord.x, minSpaceCoord.y), false);
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
	mViewGroupState->SetZoomLevel(new_zoom);

	if (OmLocalPreferences::getStickyCrosshairMode()){
		SetViewSliceOnPan();
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
		return;
	}

        // Update the pan so view stays centered.
        ViewType vts[] = { XY_VIEW, YZ_VIEW, XZ_VIEW };

        for (int i = 0; i < 3; i++) {
                Vector2 < int >pro_zoom = mViewGroupState->GetZoomLevel();
                int widthTranslate = mViewGroupState->GetPanDistance(vts[i]).x;
                int heightTranslate = mViewGroupState->GetPanDistance(vts[i]).y;

                if (pro_zoom.x > new_zoom.x) {
                        widthTranslate = widthTranslate / 2;
                        heightTranslate = heightTranslate / 2;
                } else if (pro_zoom.x < new_zoom.x) {
                        widthTranslate = widthTranslate * 2;
                        heightTranslate = heightTranslate * 2;
                }

                mViewGroupState->SetPanDistance(vts[i], Vector2 < int >(widthTranslate, heightTranslate),
                                                           postEvent);
        }
	SetViewSliceOnPan();
}

void OmView2d::setBrushToolDiameterUp()
{
	switch(mViewGroupState->getView2DBrushToolDiameter()){
	case 1:
		mViewGroupState->setView2DBrushToolDiameter(2);
		break;
	case 2:
		mViewGroupState->setView2DBrushToolDiameter(8);
		break;
	case 8:
		mViewGroupState->setView2DBrushToolDiameter(16);
		break;
	case 16:
		mViewGroupState->setView2DBrushToolDiameter(32);
		break;
	case 32:
		mViewGroupState->setView2DBrushToolDiameter(64);
		break;
	case 64:
		mViewGroupState->setView2DBrushToolDiameter(64);
		break;
	}
}

void OmView2d::setBrushToolDiameterDown()
{
	switch(mViewGroupState->getView2DBrushToolDiameter()){
	case 1:
		mViewGroupState->setView2DBrushToolDiameter(1);
		break;
	case 2:
		mViewGroupState->setView2DBrushToolDiameter(1);
		break;
	case 8:
		mViewGroupState->setView2DBrushToolDiameter(2);
		break;
	case 16:
		mViewGroupState->setView2DBrushToolDiameter(8);
		break;
	case 32:
		mViewGroupState->setView2DBrushToolDiameter(16);
		break;
	case 64:
		mViewGroupState->setView2DBrushToolDiameter(32);
		break;
	}
}

void OmView2d::MoveUpStackCloserToViewer()
{
	const int depth = GetDepthToDataSlice(mViewType);
	const int mipLevel = mViewGroupState->GetZoomLevel().x;
	const int numberOfSlicestoAdvance = OMPOW(2, mipLevel);

	SetDataSliceToDepth(mViewType, depth+numberOfSlicestoAdvance);
}

void OmView2d::MoveDownStackFartherFromViewer()
{
	const int depth = GetDepthToDataSlice(mViewType);
	const int mipLevel = mViewGroupState->GetZoomLevel().x;
	const int numberOfSlicestoAdvance = OMPOW(2, mipLevel);

	SetDataSliceToDepth(mViewType, depth-numberOfSlicestoAdvance);

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

void OmView2d::SegmentObjectModificationEvent(OmSegmentEvent*)
{
	//add/remove segment, change state, change selection
	//valid methods: GetModifiedSegmentIds()

	//Refresh ();
	myUpdate();
}

void OmView2d::SegmentDataModificationEvent()
{
	//voxels of a segment have changed
	//valid methods: GetModifiedSegmentIds()

	debug("view2d","OmView2d::SegmentDataModificationEvent\n");
}

void OmView2d::SegmentEditSelectionChangeEvent()
{
	//change segment edit selection

	debug("view2d","OmView2d::SegmentEditSelectionChangeEvent\n");

	if (mVolumeType == SEGMENTATION) {
		// need to myUpdate paintbrush, not anything on the screen

		SegmentDataWrapper sdw = OmSegmentEditor::GetEditSelection();

		if( sdw.isValid()) {
			if (sdw.getSegmentationID() == mImageId) {
				const OmColor & color = sdw.getColorInt();
				editColor = qRgba(color.red, color.green, color.blue, 255);
			}
		}
	}
}

/////////////////////////////////
///////          Actions

void OmView2d::myUpdate()
{
	if (mDoRefresh) {
		if(mEditedSegmentation) {
                	(new OmVoxelSetValueAction(mEditedSegmentation, mUpdateCoordsSet, mCurrentSegmentId))->Run();
                	mUpdateCoordsSet.clear();
			mEditedSegmentation = 0;
		}
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

void OmView2d::DrawCursors()
{
	// convert mCenter to data coordinates
	SpaceCoord depth;
	depth.x = mViewGroupState->GetViewSliceDepth(YZ_VIEW);
	depth.y = mViewGroupState->GetViewSliceDepth(XZ_VIEW);
	depth.z = mViewGroupState->GetViewSliceDepth(XY_VIEW);


	ScreenCoord screenc = SpaceToScreenCoord(mViewType,depth);
	//debug("cross","Crosshair Screen: %i %i \n",screenc.x, screenc.y);
	switch (mViewType) {
	case XY_VIEW:
		painter.setPen(Qt::red);
		painter.drawLine(mTotalViewport.width/2,0,mTotalViewport.width/2,mTotalViewport.height);
		painter.setPen(Qt::green);
       		painter.drawLine(0,mTotalViewport.height/2,mTotalViewport.width,mTotalViewport.height/2);
		break;
	case XZ_VIEW:
		painter.setPen(Qt::red);
 		painter.drawLine(mTotalViewport.width/2,0,mTotalViewport.width/2,mTotalViewport.height);
		painter.setPen(Qt::blue);
       		painter.drawLine(0,mTotalViewport.height/2,mTotalViewport.width,mTotalViewport.height/2);
		break;
	case YZ_VIEW:
		painter.setPen(Qt::blue);
		painter.drawLine(mTotalViewport.width/2,0,mTotalViewport.width/2,mTotalViewport.height);
		painter.setPen(Qt::green);
       		painter.drawLine(0,mTotalViewport.height/2,mTotalViewport.width,mTotalViewport.height/2);
		break;
	}
}

bool OmView2d::amInFillMode()
{
	bool inFill = OmStateManager::GetToolMode() == FILL_MODE;
	return inFill;
}

bool OmView2d::doDisplayInformation()
{
	return OmPreferences::GetBoolean(OM_PREF_VIEW2D_SHOW_INFO_BOOL);
}

QSize OmView2d::sizeHint () const
{
	return OmStateManager::getViewBoxSizeHint();
}

void OmView2d::ToolModeChangeEvent()
{
	OmCursors::setToolCursor(this);
}
