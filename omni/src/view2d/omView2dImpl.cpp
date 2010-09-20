#include "project/omProject.h"
#include "system/omGarbage.h"
#include "system/omLocalPreferences.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "view2d/drawable.h"
#include "view2d/omCachingThreadedCachingTile.h"
#include "view2d/omView2dImpl.h"
#include "volume/omChannel.h"

OmView2dImpl::OmView2dImpl(QWidget * parent)
	: QWidget(parent)
	, drawComplete(false)
{
}

void OmView2dImpl::Draw()
{
	drawComplete = true;

	Vector2f zoomMipVector = mViewGroupState->GetZoomLevel();

	PreDraw(zoomMipVector);

	TextureDraw(mTextures);
	mTextures.clear ();
}

void OmView2dImpl::PreDraw(Vector2f zoomMipVector)
{
	drawComplete = true;
	unsigned int freshness = 0;

	Vector2f translateVector = GetPanDistance(mViewType);
	float zoomFactor = (zoomMipVector.y / 10.0);

	Vector3f depth = Vector3f( 0, 0, 0);
	DataCoord data_coord;
	int mDataDepth = 0;
	switch (mViewType){
	case XY_VIEW:
		depth.z = mViewGroupState->GetViewSliceDepth(XY_VIEW);
		data_coord = SpaceToDataCoord(depth);
	        mDataDepth = data_coord.z;
		break;
	case XZ_VIEW:
		depth.y = mViewGroupState->GetViewSliceDepth(XZ_VIEW);
		data_coord = SpaceToDataCoord(depth);
	        mDataDepth = data_coord.y;
		break;
	case YZ_VIEW:
		depth.x = mViewGroupState->GetViewSliceDepth(YZ_VIEW);
		data_coord = SpaceToDataCoord(depth);
	        mDataDepth = data_coord.x;
		break;
	}

	float tileLength = 0;
	switch (mCache->mVolType) {
	case CHANNEL:
		tileLength = OmProject::GetChannel(mCache->mImageId).GetChunkDimension();
		break;
	case SEGMENTATION:
		tileLength = OmProject::GetSegmentation(mCache->mImageId).GetChunkDimension();
		freshness = OmCachingThreadedCachingTile::Freshen(false);
		break;
	}

	bool complete = true;
	float xMipChunk;
	float yMipChunk;
	float xval;
	float yval;
	Vector2f stretch = mVolume->GetStretchValues(mViewType);

	float pl = OMPOW(2, zoomMipVector.x);
	int tl = tileLength * OMPOW(2, zoomMipVector.x);

	if (translateVector.y < 0) {
		yMipChunk = ((abs((int)translateVector.y) /tl)) * tl * pl;
		yval = (-1 * (abs((int)translateVector.y) % tl));
	} else {
		yMipChunk = 0;
		yval = translateVector.y;
	}

	for (float y = yval; y < (mTotalViewport.height/zoomFactor/stretch.y);
	     y = y + tileLength, yMipChunk = yMipChunk + tl) {

		if (translateVector.x < 0) {
			xMipChunk = ((abs((float)translateVector.x) / tl)) * tl * pl;
			xval = (-1 * (abs((float)translateVector.x) % tl));
		} else {
			xMipChunk = 0;
			xval = translateVector.x;
		}

		for (float x = xval; x < (mTotalViewport.width * (1.0 / zoomFactor/stretch.x));
		     x = x + tileLength, xMipChunk = xMipChunk + tl) {

			DataCoord this_data_coord = ToDataCoord(xMipChunk, yMipChunk, mDataDepth);;
			SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);

			OmTileCoord mTileCoord = OmTileCoord(zoomMipVector.x, this_space_coord, mVolumeType, freshness);
			//printf("showing %f, %f,%f,%f, %i, %i\n", zoomMipVector.x, DEBUGV3(this_space_coord), mVolumeType, freshness);
			//printf("showing %f, %i,%i,%i, %i, %i\n", zoomMipVector.x, DEBUGV3(this_data_coord), mVolumeType, freshness);
			NormCoord mNormCoord = mVolume->SpaceToNormCoord(mTileCoord.Coordinate);
			OmMipChunkCoord coord = mCache->mVolume->NormToMipCoord(mNormCoord, mTileCoord.Level);

			OmTextureIDPtr gotten_id;
                        if (mCache->mVolume->ContainsMipChunkCoord(coord)) {
				mCache->GetTextureID(gotten_id, mTileCoord, false);
				mTileCount++;
				if (gotten_id) {
					safeTexture(gotten_id);
					mTextures.push_back(new Drawable(x*stretch.x, y*stretch.y, tileLength, mTileCoord, zoomFactor, gotten_id));
				} else if(mScribbling) {
                                	mCache->GetTextureID(gotten_id, mTileCoord, true);
                                        safeTexture(gotten_id);
					mTextures.push_back(new Drawable(x*stretch.x, y*stretch.y, tileLength, mTileCoord, zoomFactor, gotten_id));
				} else {
					mTileCountIncomplete++;
					complete = false;
				}
			}
			//			else debug("predrawverbose", "bad coordinates\n");
		}
	}
	if (!complete) {
		//		debug ("spin", "not complete yet in predraw\n");
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
	}
}

Vector2f OmView2dImpl::GetPanDistance(ViewType viewType)
{
        if(OmLocalPreferences::getStickyCrosshairMode()){
		return GetPanDistanceStickyMode(viewType);
	}

 	return mViewGroupState->GetPanDistance(mViewType);
}

inline Vector2f OmView2dImpl::GetPanDistanceStickyMode(ViewType viewType)
{
        Vector2f mZoomLevel = mViewGroupState->GetZoomLevel();
        Vector2f stretch= mVolume->GetStretchValues(mViewType);
        float factor = OMPOW(2,mZoomLevel.x);
        float zoomScale = mZoomLevel.y;

	float x = mViewGroupState->GetViewSliceDepth(YZ_VIEW);
	float y = mViewGroupState->GetViewSliceDepth(XZ_VIEW);
	float z = mViewGroupState->GetViewSliceDepth(XY_VIEW);

	SpaceCoord mydepth = SpaceCoord(x, y, z);
	DataCoord mydataCoord = SpaceToDataCoord(mydepth);
	//	debug("pan", "1: dc:x,y,z:%i,%i,%i\n", mydataCoord.x, mydataCoord.y, mydataCoord.z);

	float panx = (mTotalViewport.width/2.0)/(zoomScale*stretch.x/10.0);
	float pany = (mTotalViewport.height/2.0)/(zoomScale*stretch.y/10.0);

	//	debug("pan", "pan:x,y:%f,%f\n", panx, pany);

	Vector2f better;

        switch(viewType){
        case XY_VIEW:
		better.x = (panx-mydataCoord.x/factor);
		better.y = (pany-mydataCoord.y/factor);
		break;
	case XZ_VIEW:
		better.x = (panx-mydataCoord.x/factor);
		better.y = (pany-mydataCoord.z/factor);
		break;
	case YZ_VIEW:
		better.x = (panx-mydataCoord.z/factor);
		better.y = (pany-mydataCoord.y/factor);
		break;
	}

	//	debug("pan", "better: x,y:%f,%f\n", better.x, better.y);

	return better;
}

DataCoord OmView2dImpl::SpaceToDataCoord(const SpaceCoord & spacec)
{
	return mVolume->SpaceToDataCoord(spacec);
}

SpaceCoord OmView2dImpl::DataToSpaceCoord(const DataCoord & datac)
{
	return mVolume->DataToSpaceCoord(datac);
}

DataCoord OmView2dImpl::ToDataCoord(float xMipChunk, float yMipChunk, float mDataDepth)
{
	switch (mViewType) {
	case XY_VIEW:
		return DataCoord(xMipChunk, yMipChunk, mDataDepth);
	case XZ_VIEW:
		return DataCoord(xMipChunk, mDataDepth, yMipChunk);
	case YZ_VIEW:
		return DataCoord(mDataDepth, yMipChunk, xMipChunk);
	}

	assert(0);
}

void OmView2dImpl::safeTexture(OmTextureIDPtr gotten_id)
{
	if(gotten_id->needToBuildTexture()){
		doSafeTexture(gotten_id);
	}
}

void OmView2dImpl::doSafeTexture(OmTextureIDPtr gotten_id)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	const GLint format = getFormat(gotten_id);
	glTexImage2D(GL_TEXTURE_2D, 0,
		     format,
		     gotten_id->getWidth(), gotten_id->getHeight(),
		     0,
		     format,
		     GL_UNSIGNED_BYTE,
		     gotten_id->getTileData());

	gotten_id->setFlag(OMTILE_GOOD);
	gotten_id->setTextureID(textureID);
	gotten_id->deleteTileData();
}

GLint OmView2dImpl::getFormat(OmTextureIDPtr gotten_id)
{
	switch(gotten_id->getFlag()){
	case OMTILE_NEEDCOLORMAP:
		return GL_RGBA;
	case OMTILE_NEEDTEXTUREBUILT:
		return GL_LUMINANCE;
	default:
		throw OmArgException("unknown flag");
	}
}

void OmView2dImpl::TextureDraw(std::vector < Drawable * >&textures)
{
	FOR_EACH(it, textures){
		Drawable* d = *it;
		if (d->mGood) {
			safeDraw(d->zoomFactor, d->x, d->y, d->tileLength, d->gotten_id);
			delete (d);
		} else {
			// Find a mip map that can be drawn in for now...TODO.
		}
	}
}

void OmView2dImpl::safeDraw(float zoomFactor, float x, float y,
			    int tileLength, OmTextureIDPtr gotten_id)
{
	Vector2f stretch = mVolume->GetStretchValues(mViewType);

	if (mViewType == YZ_VIEW) {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.5, 0.5, 0.0);
		glRotatef(-90, 0.0, 0.0, 1.0);
		glTranslatef(-0.5, -0.5, 0.0);
		glMatrixMode(GL_MODELVIEW);
	}

	glBindTexture(GL_TEXTURE_2D, gotten_id->getTextureID());
	glBegin(GL_QUADS);

	GLfloat xLowerLeft, yLowerLeft, xLowerRight, yLowerRight, xUpperRight,
		yUpperRight, xUpperLeft, yUpperLeft;

	if (mViewType == XY_VIEW) {
		xLowerLeft  = x * zoomFactor;
		yLowerLeft  = y * zoomFactor;
		xLowerRight = (x + tileLength*stretch.x) * zoomFactor;
		yLowerRight = y * zoomFactor;
		xUpperRight = (x + tileLength*stretch.x) * zoomFactor;
		yUpperRight = (y + tileLength*stretch.y) * zoomFactor;
		xUpperLeft  = x * zoomFactor;
		yUpperLeft  = (y + tileLength*stretch.y) * zoomFactor;
	} else if (mViewType == XZ_VIEW) {
		xLowerLeft  = x * zoomFactor;
		yLowerLeft  = y * zoomFactor;
		xLowerRight = (x + tileLength*stretch.x) * zoomFactor;
		yLowerRight = y * zoomFactor;
		xUpperRight = (x + tileLength*stretch.x) * zoomFactor;
		yUpperRight = (y + tileLength*stretch.y) * zoomFactor;
		xUpperLeft  = x * zoomFactor;
		yUpperLeft  = (y + tileLength*stretch.y) * zoomFactor;
	} else if (mViewType == YZ_VIEW) {
		xLowerLeft  = (x + tileLength*stretch.x) * zoomFactor;
		yLowerLeft  = y * zoomFactor;
		xLowerRight = x * zoomFactor;
		yLowerRight = y * zoomFactor;
		xUpperRight = x * zoomFactor;
		yUpperRight = (y + tileLength*stretch.y) * zoomFactor;
		xUpperLeft  = (x + tileLength*stretch.x) * zoomFactor;
		yUpperLeft  = (y + tileLength*stretch.y) * zoomFactor;
	}

	glTexCoord2f(0.0f, 0.0f);	/* lower left corner of image */
	glVertex2f(xLowerLeft, yLowerLeft);

	glTexCoord2f(1.0f, 0.0f);	/* lower right corner of image */
	glVertex2f(xLowerRight, yLowerRight);

	glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
	glVertex2f(xUpperRight, yUpperRight);

	glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
	glVertex2f(xUpperLeft, yUpperLeft);
	glEnd();
}

QImage OmView2dImpl::safePaintEvent()
{
	mTextures.clear();

	mTileCount = 0;
	mTileCountIncomplete = 0;
	initializeGL();
	pbuffer->makeCurrent();

	//glDepthMask(true);
	//glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(0.0, 0.0, 0.0, 1.0);
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

			OmChannel & current_channel = OmProject::GetChannel(mImageId);
			foreach( OmId id, current_channel.GetValidFilterIds() ) {
        			OmFilter2d &filter = current_channel.GetFilter( id );

				alpha = filter.GetAlpha();
				glEnable(GL_BLEND);	// enable blending for transparency
				glBlendFunc(GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);
#if WIN32
				mGlBlendColorFunction(1.f, 1.f, 1.f, (1.f - alpha));
#else
				glBlendColor(1.f, 1.f, 1.f, (1.f - alpha));
#endif

				DrawFromFilter(filter);
				glDisable(GL_BLEND);	// enable blending for transparency
			}

		} else {
			DrawFromCache();
		}
	}

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	pbuffer->doneCurrent();
	return pbuffer->toImage();
}

// IMPORTANT: To cooperate fully with QPainter, we defer matrix
//   stack operations and attribute initialization until
//   the widget needs to be myUpdated.
// The initializeGL() function is called just once, before paintGL() is called.
// More importantly this function is called before "make current" calls.
void OmView2dImpl::initializeGL()
{
	mTotalViewport.lowerLeftX = 0;
	mTotalViewport.lowerLeftY = 0;
	mTotalViewport.width = size().width();
	mTotalViewport.height = size().height();

	mNearClip = -1;
	mFarClip = 1;
	mZoomLevel = 0;
}

void OmView2dImpl::DrawFromCache()
{
	if (mVolumeType == CHANNEL) {
		OmChannel & current_channel = OmProject::GetChannel(mImageId);
		mVolume = &current_channel;

		OmCachingThreadedCachingTile *fastCache =
			new OmCachingThreadedCachingTile(mViewType, mVolumeType, mImageId, &current_channel, NULL, mViewGroupState);
		mCache = fastCache->mCache;
		if (fastCache->mDelete)
			delete fastCache;

		Draw();
	} else {
		OmSegmentation & current_seg = OmProject::GetSegmentation(mImageId);
		mVolume = &current_seg;
		OmCachingThreadedCachingTile *fastCache =
			new OmCachingThreadedCachingTile(mViewType, mVolumeType, mImageId, mVolume, NULL, mViewGroupState);
		mCache = fastCache->mCache;
		if (fastCache->mDelete)
			delete fastCache;

		Draw();
	}
}

void OmView2dImpl::DrawFromFilter(OmFilter2d &filter)
{
	OmThreadedCachingTile *cache = filter.GetCache(mViewType, mViewGroupState);
	if (!cache)
		return;

	OmThreadedCachingTile *sCache = mCache;
	OmMipVolume * sVolume = mVolume;

	mCache = cache;
	mVolume = cache->mVolume;


	double alpha = mAlpha;
	mAlpha = filter.GetAlpha();

	Draw();

	mAlpha = alpha;
	mCache = sCache;
	mVolume = sVolume;
}

