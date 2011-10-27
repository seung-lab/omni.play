#pragma once

#include "common/debug.h"
#include "network/jpeg.h"
#include "tiles/tile.h"
#include "volume/segmentationDataWrapper.hpp"
#include "volume/channelDataWrapper.hpp"
#include "utility/timer.hpp"
#include "utility/UUID.hpp"
#include "volume/volume.h"
#include "zi/mutex.h"

namespace om {
namespace network {

class writeTile {
private:
    const volume::ChannelDataWrapper cdw_;
    const volume::SegmentationDataWrapper sdw_;

public:
    writeTile()
        : cdw_(1)
        , sdw_(1)
    {}

    std::string MakeTileFileChannel(const coords::globalCoord& coord, int mipLevel)
    {
        const utility::UUID uuid;

        {
            utility::timer timer;

            makeImgFile(coord, uuid, cdw_, mipLevel);

            timer.Print("\tdone making channel tile");
        }

        return uuid.Str();
    }

    std::string MakeTileFileSegmentation(const coords::globalCoord& coord, int mipLevel)
    {
        const utility::UUID uuid;

        {
            utility::timer timer;

            makeImgFile(coord, uuid, sdw_, mipLevel);

            timer.Print("\tdone making segmentation tile");
        }

        return uuid.Str();
    }

private:
    template <typename T>
    std::string fileName(T* vol, const utility::UUID& uuid){
        return doMakeFileName(vol->GetNameHyphen(), uuid);
    }

    std::string doMakeFileName(const std::string volNameHyphen, const utility::UUID& uuid)
    {
        return "/var/www/temp_omni_imgs/" + volNameHyphen + "/" +
            uuid.Str() + ".tile.jpg";
    }

    void makeImgFile(const coords::globalCoord& coord, const utility::UUID& uuid,
                     const volume::ChannelDataWrapper& wrapper, int mipLevel)
    {
        if(wrapper.IsValidWrapper()){
            makeImgFileValid<volume::channel, uint8_t>(coord, uuid,
                                                       wrapper.GetChannelPtr(), mipLevel);
        } else {
            throw common::argException("bad data wrapper.");
        }
    }

    void makeImgFile(const coords::globalCoord& coord, const utility::UUID& uuid,
                     const volume::SegmentationDataWrapper& wrapper, int mipLevel)
    {
        if(wrapper.IsValidWrapper()){
            makeImgFileValid<volume::segmentation, common::colorARGB>
                (coord, uuid, wrapper.GetSegmentationPtr(), mipLevel);
        } else {
            throw common::argException("bad data wrapper.");
        }
    }

    template <typename T, typename U>
    void makeImgFileValid(const coords::globalCoord& coord, const utility::UUID& uuid,
                          T* vol, int mipLevel)
    {
        boost::shared_ptr<tiles::tile> tilePtr = getTile(vol, coord, mipLevel);
        tilePtr->loadData();
        U const*const tileData = tilePtr->data<U>();

        const std::string fname = fileName(vol, uuid);

        writeJPEG(tileData, fname);
    }

    void writeJPEG(uint8_t const*const data, const std::string& fname) const {
        om::jpeg::write8bit(128, 128, data, fname);
    }

    void writeJPEG(common::colorARGB const*const data, const std::string& fname) const {
        om::jpeg::writeRGB(128, 128, reinterpret_cast<uint8_t const*const>(data), fname);
    }

    template <typename T>
    boost::shared_ptr<tiles::tile> getTile(T* vol,
                                              const coords::globalCoord& coord,
                                              int mipLevel)
    {
        coords::chunkCoord chunkCoord = coord.toChunkCoord(*vol, mipLevel);
        return boost::make_shared<tiles::tile>(vol, chunkCoord, common::XY_VIEW, coord.z);
        // TODO: Generalize to different views
    }
};

}
}
