#ifndef OM_VIEW_2D_IMPL_H
#define OM_VIEW_2D_IMPL_H

#include "common/omCommon.h"
#include "view2d/omTile.h"

#include <QImage>
#include <QGLPixelBuffer>
#include <QtGui>

class OmFilter2d;
class Drawable;
class OmTextureID;
class OmThreadedCachingTile;
class OmViewGroupState;

class OmView2dImpl : public QWidget
{
 Q_OBJECT

 protected:
	OmView2dImpl(QWidget *);

	void Draw();
	Vector2f GetPanDistance(ViewType viewType);
	DataCoord SpaceToDataCoord(const SpaceCoord &spacec);
	SpaceCoord DataToSpaceCoord(const DataCoord &datac);
        DataCoord ToDataCoord(float, float, float);

	bool drawComplete;
	ViewType mViewType;
	OmThreadedCachingTile *mCache;
	OmMipVolume *mVolume;
	OmViewGroupState * mViewGroupState;
	ObjectType mVolumeType;
	OmId mImageId;
	int mTileCount;
	bool mScribbling;
	int mTileCountIncomplete;
	int mRootLevel;
	int mZoomLevel;
	double mAlpha;

	vector <Drawable*> mTextures;

	Vector4i mTotalViewport; //lower left x, lower left y, width, height

	int mNearClip;
	int mFarClip;
	QGLPixelBuffer * pbuffer;
	QImage safePaintEvent();	// pbuffered paint.

 private:
	void initializeGL();

	void DrawFromCache();
	void DrawFromFilter(OmFilter2d&);

	void PreDraw(Vector2f zoomMipVector);
	void TextureDraw(vector <Drawable*> &textures);
	void safeDraw(float zoomFactor, float x, float y, int tileLength,
		      OmTextureIDPtr gotten_id);

	void safeTexture(OmTextureIDPtr gotten_id);
	void doSafeTexture(OmTextureIDPtr gotten_id);
	GLint getFormat(OmTextureIDPtr gotten_id);

	Vector2f GetPanDistanceStickyMode(ViewType viewType);
};

#endif
