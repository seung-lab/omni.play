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
{
	// drawComplete = true; // this was never initialized! set it here?

	setBackgroundColor();
}

void OmView2dImpl::setBackgroundColor()
{
	// http://wiki.forum.nokia.com/index.php/CS001348_-_Changing_QWidget_background_colour
	// Set background colour to black
	QPalette p(palette());
	p.setColor(QPalette::Background, Qt::black);
	setPalette(p);
}

void OmView2dImpl::Draw()
{
	drawComplete = true;

	Vector2f zoomMipVector = mViewGroupState->GetZoomLevel();
	if (0) {
		Vector2f zoom = zoomMipVector;
		Vector2f translateVector = GetPanDistance(mViewType);

		int lvl = zoomMipVector.x+1;

		for (int i = mRootLevel; i > lvl; --i) {

			zoom.x = i;
			zoom.y = zoomMipVector.y * (1 + i - zoomMipVector.x);
			debug("view2d","OmView2d::Draw(zoom lvl %i, scale %i\n)\n");

			mViewGroupState->SetPanDistance(mViewType,
							Vector2f(translateVector.x / (1 + i - zoomMipVector.x),
								 translateVector.y / (1 + i - zoomMipVector.x)),
							false);

			PreDraw(zoom);
		}
		mViewGroupState->SetPanDistance(mViewType,
							   Vector2f(translateVector.x, translateVector.y),
							   false);
	}

	PreDraw(zoomMipVector);
	if (mDrawFromChannel) {
		OmStateManager::SetViewDrawable(mViewType, mThreeTextures);
	}
	mThreeTextures.clear();
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
	case VOLUME:
	case SEGMENT:
	case NOTE:
	case FILTER:
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
			NormCoord mNormCoord = mVolume->SpaceToNormCoord(mTileCoord.Coordinate);
			OmMipChunkCoord coord = mCache->mVolume->NormToMipCoord(mNormCoord, mTileCoord.Level);
			/*
			debug ("postdraw", "this_data_coord.(x,y,z): (%i,%i,%i)\n", this_data_coord.x,this_data_coord.y,this_data_coord.z);
			debug ("postdraw", "this_space_coord.(x,y,z): (%f,%f,%f)\n", this_space_coord.x,this_space_coord.y,this_space_coord.z);
			debug ("postdraw", "coord.(x,y,z): (%f,%f,%f)\n", coord.Coordinate.x,coord.Coordinate.y,coord.Coordinate.z);
			*/
			OmTextureIDPtr gotten_id;
                        if (mCache->mVolume->ContainsMipChunkCoord(coord)) {
				mCache->GetTextureID(gotten_id, mTileCoord, false);
				mTileCount++;
				if (gotten_id) {
					safeTexture(gotten_id);
					mTextures.push_back(new Drawable(x*stretch.x, y*stretch.y, tileLength, mTileCoord, zoomFactor, gotten_id));
					mThreeTextures.push_back(new Drawable(x*stretch.x, y*stretch.y, tileLength, mTileCoord, zoomFactor, gotten_id));
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
	} else {
		BufferTiles(zoomMipVector);
		//debug ("genone", "complete in predraw\n");
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

bool OmView2dImpl::BufferTiles(Vector2f zoomMipVector)
{
#define BUFFERCOUNT 10
	int boff[BUFFERCOUNT] = {5, -5, 4, -4, 3, -3, 2, -2, 1, -1};

	drawComplete = true;
	unsigned int freshness = 0;
	//debug("genone","OmView2d::Draw(zoom lvl %i, scale %i)\n", zoomMipVector.x, zoomMipVector.y);

	//zoomMipVector = mViewGroupState->GetZoomLevel();

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
	case VOLUME:
	case SEGMENT:
	case NOTE:
	case FILTER:
		break;
	}

	bool complete = true;
	debug("genone", "in buffering: %i\n", mDataDepth);
	for (int count = 0; count < BUFFERCOUNT; count++) {
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

		//printf("count=%i\n", count);
		for (float y = yval; y < (mTotalViewport.height/zoomFactor/stretch.y);
	     		y = y + tileLength, yMipChunk = yMipChunk + tl) {

			if (translateVector.x < 0) {
				xMipChunk = ((abs((float)translateVector.x) / tl)) * tl * pl;
				xval = (-1 * (abs((float)translateVector.x) % tl));
			} else {
				xMipChunk = 0;
				xval = translateVector.x;
			}

#if 0
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

			for (float x = xval; x < (mTotalViewport.width * (1.0 / zoomFactor/stretch.x));
		     			x = x + tileLength, xMipChunk = xMipChunk + tl) {

                        	DataCoord this_data_coord = ToDataCoord(xMipChunk, yMipChunk, mDataDepth+boff[count]);
                        	SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
                        	OmTileCoord mTileCoord = OmTileCoord(zoomMipVector.x, this_space_coord, mVolumeType, freshness);
                        	NormCoord mNormCoord = mVolume->SpaceToNormCoord(mTileCoord.Coordinate);
                        	OmMipChunkCoord coord = mCache->mVolume->NormToMipCoord(mNormCoord, mTileCoord.Level);
				OmTextureIDPtr gotten_id;
                        	if (mCache->mVolume->ContainsMipChunkCoord(coord)) {
                                	mCache->GetTextureID(gotten_id, mTileCoord, false);
					debug("genone", "buffering: %i, %i\n", count, boff[count]);
                                	if (gotten_id) {
                                        	safeTexture(gotten_id);
					} else {
						mTileCountIncomplete++;
                                        	if (mTileCountIncomplete >= mTileCount) {
							OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
							return false;
						}
                                        	complete = false;
                                	}
				}
			}
		}
	}

	if (!complete) {
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
	}

	debug("genone", "done buffering\n");
	return complete;
}


void OmView2dImpl::TextureDraw(vector < Drawable * >&textures)
{
	Drawable * d;
	vector < Drawable * >::iterator it;
	for(it = textures.begin(); it != textures.end(); ++it){
		d = *it;
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

	glBindTexture(GL_TEXTURE_2D, gotten_id->GetTextureID());
	glBegin(GL_QUADS);

	GLfloat xLowerLeft, yLowerLeft, xLowerRight, yLowerRight, xUpperRight, yUpperRight, xUpperLeft, yUpperLeft;

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

// The initializeGL() function is called just once, before paintGL() is called.
// More importantly this function is called before "make current" calls.
void OmView2dImpl::initializeGL()
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

void OmView2dImpl::DrawFromCache()
{
	if (mVolumeType == CHANNEL) {
		mDrawFromChannel = true;
		OmChannel & current_channel = OmProject::GetChannel(mImageId);
		mVolume = &current_channel;

		OmCachingThreadedCachingTile *fastCache =
			new OmCachingThreadedCachingTile(mViewType, mVolumeType, mImageId, &current_channel, NULL, mViewGroupState);
		mCache = fastCache->mCache;
		if (fastCache->mDelete)
			delete fastCache;

		Draw();
		mDrawFromChannel = false;
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

