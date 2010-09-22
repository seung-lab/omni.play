#ifndef OM_TILE_DRAWER_HPP
#define OM_TILE_DRAWER_HPP

#include "tiles/omTileTypes.hpp"

class OmMipVolume;
class OmView2dState;
class OmFilter2d;

class OmTileDrawer{
public:
	OmTileDrawer(boost::shared_ptr<OmView2dState> v2ds,
		     const ViewType vt);
	~OmTileDrawer();

	void fullRedraw();

	int getTileCount(){
		return mTileCount;
	}
	int getTileCountIncomplete(){
		return mTileCountIncomplete;
	}
	bool isDrawComplete(){
		return !mTileCountIncomplete;
	}

	const boost::shared_ptr<OmView2dState>& getState(){
		return state_;
	}

private:
	boost::shared_ptr<OmView2dState> state_;
	const ViewType mViewType;

	int mTileCount;
	int mTileCountIncomplete;

	std::deque<OmTileAndVertices> mTileToDraw;

	void reset();

	void determineWhichTilesToDraw(OmMipVolume* vol);
	void draw(OmMipVolume* vol);
	void drawFromFilter(OmFilter2d& filter);

	OmTileCoordsAndLocationsPtr
	getTileCoordsAndLocationsForCurrentScene(OmMipVolume* vol);

	void drawTiles();
	void drawTile(const OmTileAndVertices& tv);

	void bindTileDataToGLid(const OmTextureIDPtr&);
	void doBindTileDataToGLid(const OmTextureIDPtr&);

	void setupGLblendColor(const float alpha);
	void teardownGLblendColor();

#ifdef WIN32
	typedef void (*GLCOLOR)(GLfloat, GLfloat, GLfloat, GLfloat);
	GLCOLOR mGlBlendColorFunction;
#endif
};

#endif
