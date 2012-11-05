#pragma once

#include "tiles/omTileTypes.hpp"
#include "view2d/om2dPreferences.hpp"
#include "view2d/omBlockingGetTiles.hpp"
#include "view2d/omCalcTileCoordsDownsampled.hpp"
#include "view2d/omOnScreenTileCoords.h"
#include "view2d/omOpenGLTileDrawer.hpp"
#include "view2d/omOpenGLUtils.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"

class OmTileDrawer{
public:
    OmTileDrawer(OmView2dState* state, const om::common::ViewType viewType)
        : state_(state)
        , viewType_(viewType)
        , blockingRedraw_(false)
        , tileCount_(0)
        , tileCountIncomplete_(0)
        , openglTileDrawer_(new OmOpenGLTileDrawer())
        , tileCalcDownsampled_(new OmCalcTileCoordsDownsampled(viewType))
        , blockingGetTiles_(new OmBlockingGetTiles(tilesToDraw_))
    {
        OmTileCache::RegisterDrawer(this);
    }

    ~OmTileDrawer()
    {
        OmTileCache::UnRegisterDrawer(this);
    }

    void FullRedraw2d(const bool blockingRedraw)
    {
        reset();

        blockingRedraw_ = blockingRedraw;

        OmMipVolume* vol = state_->getVol();

        if(CHANNEL == vol->getVolumeType()){
            drawChannelAndFilters(vol);

        } else {

            OmSegmentation* seg = reinterpret_cast<OmSegmentation*>(vol);
            if(!seg) {
            	throw new om::FormatException("Bad Cast to OmSegmentation.");
            }
            draw(seg);
        }
    }

    int GetTileCount(){
        return tileCount_;
    }

    int GetTileCountIncomplete(){
        return tileCountIncomplete_;
    }

    bool IsDrawComplete(){
        return 0 == tileCountIncomplete_;
    }

    OmView2dState* GetState(){
        return state_;
    }

private:
    OmView2dState* state_;
    const om::common::ViewType viewType_;

    bool blockingRedraw_;

    int tileCount_;
    int tileCountIncomplete_;
    boost::scoped_ptr<OmOpenGLTileDrawer> openglTileDrawer_;
    boost::scoped_ptr<OmCalcTileCoordsDownsampled> tileCalcDownsampled_;
    boost::scoped_ptr<OmBlockingGetTiles> blockingGetTiles_;

    std::deque<OmTileAndVertices> tilesToDraw_;
    std::deque<OmTileAndVertices> oldTilesToDraw_;

    void reset()
    {
        // keep tiles from previous draw around; this ensures we don't clear
        //  their OpenGL texture IDs before the card is done drawing the textures
        //  (this can occur if other threads clear the cache of all tiles...)
        oldTilesToDraw_.swap(tilesToDraw_);
        tilesToDraw_ = std::deque<OmTileAndVertices>();

        tileCount_ = 0;
        tileCountIncomplete_ = 0;
    }

    template <typename V>
    void draw(V* vol)
    {
        determineWhichTilesToDraw(vol);

        const bool finished = openglTileDrawer_->DrawTiles(tilesToDraw_);

        if(!finished){
            OmEvents::Redraw2d();
        }
    }

    void determineWhichTilesToDraw(OmChannel* vol)
    {
        OmTileCoordsAndLocationsPtr tileCoordsAndLocations =
        getTileCoordsAndLocationsForCurrentScene(vol);

        tileCount_ += tileCoordsAndLocations->size();

        if(blockingRedraw_){
            getTilesBlocking(tileCoordsAndLocations);

        } else {
            getTilesNonBlocking(tileCoordsAndLocations);
        }
    }

    void determineWhichTilesToDraw(OmSegmentation* vol)
    {
        OmTileCoordsAndLocationsPtr tileCoordsAndLocations =
        getTileCoordsAndLocationsForCurrentScene(vol);

        tileCount_ += tileCoordsAndLocations->size();

        if(state_->getScribbling() || blockingRedraw_){
            getTilesBlocking(tileCoordsAndLocations);

        } else {
            getTilesNonBlocking(tileCoordsAndLocations);
        }
    }

    void getTilesNonBlocking(OmTileCoordsAndLocationsPtr tileCoordsAndLocations)
    {
        static const TextureVectices defaultTextureVectices =
                    { {0.f, 1.f}, {1.f, 0.f} };

        FOR_EACH(tileCL, *tileCoordsAndLocations)
        {
            OmTilePtr tile;
            OmTileCache::Get(tile, tileCL->tileCoord, om::NON_BLOCKING);

            if(tile)
            {
                OmTileAndVertices tv = {tile,
                                        tileCL->vertices,
                                        defaultTextureVectices};
                tilesToDraw_.push_back(tv);

                // std::cout << "drawing tile: " << tileCL->tileCoord << "\n";

            } else
            {
                ++tileCountIncomplete_;
                tileCalcDownsampled_->TryDownsample(*tileCL, tilesToDraw_);
            }
        }
    }

    void getTilesBlocking(OmTileCoordsAndLocationsPtr tileCoordsAndLocations)
    {
        blockingGetTiles_->GetAll(tileCoordsAndLocations);
    }

    OmTileCoordsAndLocationsPtr
    getTileCoordsAndLocationsForCurrentScene(OmMipVolume* vol)
    {
        OmOnScreenTileCoords stc(state_, vol);
        return stc.ComputeCoordsAndLocations();
    }

    bool contains(Vector4i viewport, GLfloatBox glBox) {
        return contains(viewport, glBox.lowerLeft) ||
               contains(viewport, glBox.lowerRight) ||
               contains(viewport, glBox.upperLeft) ||
               contains(viewport, glBox.upperRight);
    }

    bool contains(Vector4i viewport, GLfloatPair glPair) {
        return viewport.x <= glPair.x && glPair.x <= viewport.width &&
        viewport.y <= glPair.y && glPair.y <= viewport.height;
    }

    void drawChannelAndFilters(OmMipVolume* vol)
    {
        const bool haveAlphaGoToBlack = Om2dPreferences::HaveAlphaGoToBlack();

        OmChannel* chan = reinterpret_cast<OmChannel*>(vol);
		if(!chan) {
        	throw new om::FormatException("Bad Cast to OmChannel.");
        }

        bool drawChannel = false;

        const std::vector<OmFilter2d*> filters = chan->GetFilters();

        FOR_EACH(iter, filters)
        {
            OmFilter2d* filter = *iter;

            const om::FilterType filterType = filter->FilterType();

            const double alpha = filter->GetAlpha();

            if(om::OVERLAY_NONE == filterType ||
               alpha < 0.05) // don't bother drawing segmentation if user won't see it
            {
                drawChannel = true;
                continue;

            }

            if (om::OVERLAY_CHANNEL == filterType)
            {
                drawChannel = true;
                OmChannel* chann = filter->GetChannel();
                draw(chann);
                continue;
            }

            OmSegmentation* seg = filter->GetSegmentation();

            draw(seg);

            const bool shouldBrightenAlpha = seg->Segments()->AreSegmentsSelected();

            om::opengl_::SetupGLblendColor(haveAlphaGoToBlack, filter->GetAlpha(),
                                           shouldBrightenAlpha);

            if(alpha < 0.95 || !haveAlphaGoToBlack)
            {
                // only draw channel if user can see it (low enough alpha,
                //     or user wants channel to disappear at high alpha)
                drawChannel = true;
            }
        }

        if(drawChannel){
            draw(chan);
        }
    }
};

