#ifndef OM_TILE_DRAWER_H
#define OM_TILE_DRAWER_H

#include "common/om.hpp"
#include "system/omGarbage.h"
#include "tiles/omTileTypes.hpp"
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
    OmTileDrawer(boost::shared_ptr<OmView2dState> state,
                 const ViewType viewType)
        : state_(state)
        , viewType_(viewType)
        , tileCount_(0)
        , tileCountIncomplete_(0)
        , openglTileDrawer_(new OmOpenGLTileDrawer(viewType))
        , tileCalcDownsampled_(new OmCalcTileCoordsDownsampled(viewType, this))
        , blockingGetTiles_(new OmBlockingGetTiles(tilesToDraw_, this))
    {
        OmTileCache::RegisterDrawer(this);

#ifdef WIN32
        mGlBlendColorFunction = (GLCOLOR) wglGetProcAddress("glBlendColor");
#endif
    }

    ~OmTileDrawer()
    {
        OmTileCache::UnRegisterDrawer(this);
    }

    void FullRedraw2d()
    {
        reset();

        OmMipVolume* vol = state_->getVol();

        if(CHANNEL == vol->getVolumeType())
        {
            OmChannel* chan = reinterpret_cast<OmChannel*>(vol);

            const std::vector<OmFilter2d*> filters = chan->GetFilters();

            FOR_EACH(iter, filters){
                filterDraw(*iter);
            }

            draw(chan);

        } else {

            OmSegmentation* seg = reinterpret_cast<OmSegmentation*>(vol);
            draw(seg);
        }

        if(IsDrawComplete() && !state_->getScribbling())
        {
            OmTileCache::SetDrawerDone(this);
            if(!OmTileCache::AreDrawersActive()){
                OmGarbage::safeCleanTextureIds();
            }
        }
    }

    int GetTileCount(){
        return tileCount_;
    }

    int GetTileCountIncomplete(){
        return tileCountIncomplete_;
    }

    bool IsDrawComplete(){
        return !tileCountIncomplete_;
    }

    const boost::shared_ptr<OmView2dState>& GetState(){
        return state_;
    }

private:
    boost::shared_ptr<OmView2dState> state_;
    const ViewType viewType_;

    int tileCount_;
    int tileCountIncomplete_;
    boost::scoped_ptr<OmOpenGLTileDrawer> openglTileDrawer_;
    boost::scoped_ptr<OmCalcTileCoordsDownsampled> tileCalcDownsampled_;
    boost::scoped_ptr<OmBlockingGetTiles> blockingGetTiles_;

    std::deque<OmTileAndVertices> tilesToDraw_;
    std::deque<OmTileAndVertices> oldTilesToDraw_;

    void reset()
    {
        // keep old tiles around to make sure tiles are not destoryed before opengl is
        //  done using their texture IDs...
        oldTilesToDraw_ = tilesToDraw_;
        tilesToDraw_.clear();

        tileCount_ = 0;
        tileCountIncomplete_ = 0;
    }

    template <typename V>
    void draw(V* vol)
    {
        if(!vol->IsBuilt()){
            return;
        }

        determineWhichTilesToDraw(vol);
        openglTileDrawer_->DrawTiles(tilesToDraw_);
    }

    void determineWhichTilesToDraw(OmChannel* vol)
    {
        OmTileCoordsAndLocationsPtr tileCoordsAndLocations =
            getTileCoordsAndLocationsForCurrentScene(vol);

        tileCount_ += tileCoordsAndLocations->size();

        getTilesNonBlocking(tileCoordsAndLocations);
    }

    void determineWhichTilesToDraw(OmSegmentation* vol)
    {
        OmTileCoordsAndLocationsPtr tileCoordsAndLocations =
            getTileCoordsAndLocationsForCurrentScene(vol);

        tileCount_ += tileCoordsAndLocations->size();

        if(state_->getScribbling()){
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
            OmTileCache::Get(this, tile, tileCL->tileCoord, om::NON_BLOCKING);

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
                // TODO
                // tileCalcDownsampled_->TryDownsample(vol, tileCL->tileCoord,
                //                                     tileCL->vertices, tilesToDraw_);
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
        OmMipVolume* curVol = state_->getVol();
        state_->setVol(vol);

        OmOnScreenTileCoords stc(state_);
        OmTileCoordsAndLocationsPtr ret = stc.ComputeCoordsAndLocations();

        state_->setVol(curVol);

        return ret;
    }

    bool drawFromFilter(OmFilter2d* filter)
    {
        const om::FilterType filterType = filter->FilterType();

        if(om::OVERLAY_NONE == filterType){
            return false;

        } else if (om::OVERLAY_CHANNEL == filterType) {
            OmChannel* chann = filter->GetChannel();
            draw(chann);
            return false;
        }

        OmSegmentation* seg = filter->GetSegmentation();
        draw(seg);
        return seg->Segments()->GetSelectedSegmentIds().empty();
    }

    void filterDraw(OmFilter2d* filter)
    {
        const bool shouldBrightenAlpha = drawFromFilter(filter);
        om::opengl_::SetupGLblendColor(filter->GetAlpha(), shouldBrightenAlpha);
    }

#ifdef WIN32
    typedef void (*GLCOLOR)(GLfloat, GLfloat, GLfloat, GLfloat);
    GLCOLOR mGlBlendColorFunction;
#endif
};

#endif

