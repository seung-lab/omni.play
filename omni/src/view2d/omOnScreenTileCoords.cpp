#include "system/cache/omCacheManager.h"
#include "tiles/omTile.h"
#include "view2d/omOnScreenTileCoords.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"
#include "volume/omMipVolume.h"

OmOnScreenTileCoords::OmOnScreenTileCoords(const boost::shared_ptr<OmView2dState>& state)
    : state_(state)
    , vol_(state->getVol())
    , viewType_(state->getViewType())
    , vgs_(state->getViewGroupState())
    , tileLength_(vol_->Coords().GetChunkDimension())
    , zoomFactor_(state->getZoomScale())
    , mipLevel_(state->getMipLevel())
    , stretch_(vol_->Coords().GetStretchValues(state->getViewType()))
    , totalViewport_(state->getTotalViewport())
    , translateVector_(state->ComputePanDistance())
    , tileCoordsAndLocations_(boost::make_shared<OmTileCoordsAndLocations>())
{
    freshness_ = 0;
    if(SEGMENTATION == vol_->getVolumeType()){
        freshness_ = OmCacheManager::GetFreshness();
    }

    setDepths();
}

std::ostream& operator<<(std::ostream &out, const OmOnScreenTileCoords& c)
{
    out << c.state_ << "\n\t";
    out << c.vol_ << "\n\t";
    out << c.viewType_ << "\n\t";
    out << c.vgs_ << "\n\t";
    out << c.tileLength_ << "\n\t";
    out << c.zoomFactor_ << "\n\t";
    out << c.mipLevel_ << "\n\t";
    out << c.stretch_ << "\n\t";
    out << c.totalViewport_ << "\n\t";
    out << c.translateVector_ << "\n\t";
    out << c.tileCoordsAndLocations_ << "\n\t";
    out << c.dataDepth_ << "\n";
    return out;
}

void OmOnScreenTileCoords::setDepths(){
    dataDepth_ = state_->Location()->DataDepth();
}

OmTileCoordsAndLocationsPtr
OmOnScreenTileCoords::ComputeCoordsAndLocations()
{
    return ComputeCoordsAndLocations(0);
}

OmTileCoordsAndLocationsPtr
OmOnScreenTileCoords::ComputeCoordsAndLocations(const int depthOffset)
{
    if(vol_->IsBuilt()){
        doComputeCoordsAndLocations(depthOffset);
    }

    OmTileCoordsAndLocationsPtr ret = tileCoordsAndLocations_;
    tileCoordsAndLocations_ = boost::make_shared<OmTileCoordsAndLocations>();
    return ret;
}

void OmOnScreenTileCoords::doComputeCoordsAndLocations(const int depthOffset)
{
    const int levelFactor = om::pow2int(mipLevel_);
    const int dataDim = tileLength_ * levelFactor;
    float xval;
    float yval;

    if(translateVector_.y < 0) {
        yMipChunk_ = ((abs((int)translateVector_.y) /dataDim)) * dataDim * levelFactor;
        yval = (-1 * (abs((int)translateVector_.y) % dataDim));
    } else {
        yMipChunk_ = 0;
        yval = translateVector_.y;
    }

    // if(!depthOffset)
    // {
    //     std::cout << "initial: yMipChunk_: " << yMipChunk_ << "; yval: " << yval << "\n";
    // }

    const float maxY = totalViewport_.height/zoomFactor_/stretch_.y;
    for(float y = yval; y < maxY; y += tileLength_, yMipChunk_ += dataDim){

        if (translateVector_.x < 0) {
            xMipChunk_ = ((abs((int)translateVector_.x) / dataDim)) * dataDim * levelFactor;
            xval = (-1 * (abs((int)translateVector_.x) % dataDim));
        } else {
            xMipChunk_ = 0;
            xval = translateVector_.x;
        }

        const float maxX = totalViewport_.width/zoomFactor_/stretch_.x;
        for(float x = xval; x < maxX; x += tileLength_, xMipChunk_ += dataDim){

            computeTile(x, y, depthOffset);
        }
    }
}

void OmOnScreenTileCoords::computeTile(const float x, const float y,
                                       const int depthOffset)
{
    const DataCoord dataCoord = toDataCoord(depthOffset);

    const OmChunkCoord chunkCoord =
        vol_->Coords().DataToMipCoord(dataCoord, mipLevel_);

    if(!vol_->Coords().ContainsMipChunkCoord(chunkCoord)){
        return;
    }

    if(depthOffset) // i.e. if we are pre-fetching
    {
        if(CHANNEL == vol_->getVolumeType())
        {
            OmChannel* chan = reinterpret_cast<OmChannel*>(vol_);

            const std::vector<OmFilter2d*> filters = chan->GetFilters();

            FOR_EACH(iter, filters){
                makeTileCoordFromFilter(*iter, dataCoord, x, y);
            }
        }
    }

    OmTileCoordAndVertices pair = {makeTileCoord(dataCoord, vol_, freshness_),
                                   computeVertices(x, y) };

    // if(!depthOffset)
    // {
    //     std::cout << "dataCoord is: " << dataCoord
    //               << "; valid mip chunk coord is: "
    //               << chunkCoord
    //               << "\n";
    //     std::cout << "(x,y)=" << x << ", " << y << "\n";
    //     std::cout << "vertices: " << pair.vertices << "\n";
    // }

    tileCoordsAndLocations_->push_back(pair);
}

void OmOnScreenTileCoords::makeTileCoordFromFilter(OmFilter2d* filter,
                                                   const DataCoord & dataCoord,
                                                   const float x, const float y)
{
    const om::FilterType filterType = filter->FilterType();

    OmMipVolume* vol = NULL;
    int freshness = 0;

    if(om::OVERLAY_NONE == filterType){
        return;

    } else if (om::OVERLAY_CHANNEL == filterType) {
        vol = filter->GetChannel();

    } else {
        vol = filter->GetSegmentation();
        freshness = OmCacheManager::GetFreshness();
    }

    OmTileCoordAndVertices pair = { makeTileCoord(dataCoord, vol, freshness),
                                    computeVertices(x, y) };

    tileCoordsAndLocations_->push_back(pair);
}

DataCoord OmOnScreenTileCoords::toDataCoord(const int depthOffset)
{
    // const Vector2f res = OmView2dConverters::Get2PtsInPlane(vol_->Coords().GetDataResolution(),
    //                                                         viewType_);

    return state_->makeViewTypeVector3<int>(xMipChunk_,
                                            yMipChunk_,
                                            dataDepth_ + depthOffset);
}

OmTileCoord OmOnScreenTileCoords::makeTileCoord(const DataCoord& coord,
                                                OmMipVolume* vol,
                                                int freshness)
{
    return OmTileCoord(mipLevel_,
                       coord,
                       vol,
                       freshness,
                       vgs_,
                       viewType_,
                       state_->getObjectType());
}

GLfloatBox OmOnScreenTileCoords::computeVertices(const float xRaw,
                                                 const float yRaw)
{
    const float x = xRaw * stretch_.x;
    const float y = yRaw * stretch_.y;

    const GLfloat minX = x * zoomFactor_;
    const GLfloat minY = y * zoomFactor_;
    const GLfloat maxX = (x + tileLength_*stretch_.x) * zoomFactor_;
    const GLfloat maxY = (y + tileLength_*stretch_.y) * zoomFactor_;

    GLfloatBox glBox;
    glBox.lowerLeft.y  = minY;
    glBox.upperRight.y = maxY;

    switch(viewType_){
    case XY_VIEW:
    case XZ_VIEW:
        glBox.lowerLeft.x  = minX;
        glBox.upperRight.x = maxX;
        break;
    case YZ_VIEW:
        glBox.lowerLeft.x  = maxX;
        glBox.upperRight.x = minX;
        break;
    }

    glBox.lowerRight.x = glBox.upperRight.x;
    glBox.lowerRight.y = glBox.lowerLeft.y;

    glBox.upperLeft.x  = glBox.lowerLeft.x;
    glBox.upperLeft.y  = glBox.upperRight.y;

    return glBox;
}
