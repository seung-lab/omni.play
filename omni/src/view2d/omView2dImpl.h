#ifndef OM_VIEW_2D_IMPL_H
#define OM_VIEW_2D_IMPL_H

#include "common/omCommon.h"

#include <QExplicitlySharedDataPointer>
#include <QImage>
#include <QGLPixelBuffer>
#include <QtGui> 

class OmFilter2d;
class Drawable;
class OmTextureID;
class OmThreadedCachingTile;
class OmMipVolume;
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
        DataCoord ToDataCoord(int xMipChunk, int yMipChunk, int mDataDepth);

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
	bool mDrawFromChannel;
	int mZoomLevel;
	double mAlpha;

	vector <Drawable*> mTextures;
	vector <Drawable*> mThreeTextures;

	Vector4i mTotalViewport; //lower left x, lower left y, width, height

	int mNearClip;
	int mFarClip;
	QGLPixelBuffer *  pbuffer;
	QImage safePaintEvent();	// pbuffered paint.

 private:
	void initializeGL();

	void DrawFromCache();
	void DrawFromFilter(OmFilter2d&);

	void PreDraw(Vector2f zoomMipVector);
	void TextureDraw(vector <Drawable*> &textures);
	void safeDraw(float zoomFactor, int x, int y, int tileLength, QExplicitlySharedDataPointer<OmTextureID> gotten_id);
	void safeTexture(QExplicitlySharedDataPointer<OmTextureID> gotten_id);
	bool BufferTiles(Vector2f zoomMipVector);

	Vector2f GetPanDistanceStickyMode(ViewType viewType);

	
};

#endif
