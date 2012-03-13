#pragma once

#include "common/omDebug.h"
#include "network/omJpeg.h"
#include "system/cache/omCacheManager.h"
#include "tiles/cache/omTileCache.h"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "utility/dataWrappers.h"
#include "utility/omTimer.hpp"
#include "utility/omUUID.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omMipVolume.h"
#include "zi/omMutex.h"

#include <QPainter>

class OmWriteTile {
private:
    OmViewGroupState vgs_;
    const ChannelDataWrapper cdw_;
    const SegmentationDataWrapper sdw_;

public:
    OmWriteTile()
        : vgs_(NULL)
        , cdw_(1)
        , sdw_(1)
    {}

    std::string MakeTileFileChannel(const om::globalCoord& coord)
    {
        const OmUUID uuid;

        {
            OmTimer timer;

            makeImgFile(coord, uuid, cdw_);

            timer.Print("\tdone making channel tile");
        }

        return uuid.Str();
    }

    std::string MakeTileFileSegmentation(const om::globalCoord& coord)
    {
        const OmUUID uuid;

        {
            OmTimer timer;

            makeImgFile(coord, uuid, sdw_);

            timer.Print("\tdone making segmentation tile");
        }

        return uuid.Str();
    }

private:
    template <typename T>
    QString fileName(T* vol, const OmUUID& uuid){
        return doMakeFileName(vol->GetNameHyphen(), uuid);
    }

    QString doMakeFileName(const std::string volNameHyphen, const OmUUID& uuid)
    {
        return QString::fromStdString(
            "/var/www/temp_omni_imgs/" + volNameHyphen + "/" +
            uuid.Str() + ".tile.jpg");
    }

    uint32_t freshness(OmChannel*){
        return 0;
    }

    uint32_t freshness(OmSegmentation*){
        return OmCacheManager::GetFreshness();
    }

    void makeImgFile(const om::globalCoord& coord, const OmUUID& uuid,
                     const ChannelDataWrapper& wrapper)
    {
        if(wrapper.IsValidWrapper()){
            makeImgFileValid<OmChannel, uint8_t>(coord, uuid,
                                                 wrapper.GetChannelPtr());
        } else {
            makeBlankImage("channel-1", uuid);
        }
    }

    void makeImgFile(const om::globalCoord& coord, const OmUUID& uuid,
                     const SegmentationDataWrapper& wrapper)
    {
        if(wrapper.IsValidWrapper()){
            makeImgFileValid<OmSegmentation, OmColorARGB>(coord, uuid,
                                                          wrapper.GetSegmentationPtr());
        } else {
            makeBlankImage("segmentation-1", uuid);
        }
    }

    void makeBlankImage(const QString volName, const OmUUID& uuid)
    {
        const QString fname = doMakeFileName(volName.toStdString(), uuid);

        QImage blank(128, 128, QImage::Format_ARGB32_Premultiplied);
        blank.fill(QColor(Qt::gray).rgb());

        if(!blank.save(fname)){
            throw OmIoException("could not write file", fname);
        }
    }

    template <typename T, typename U>
    void makeImgFileValid(const om::globalCoord& coord, const OmUUID& uuid, T* vol)
    {
        OmTilePtr tilePtr = getTile(vol, coord);
        const OmTextureID& texture = tilePtr->GetTexture();
        U const*const tileData = reinterpret_cast<U const*const>(texture.GetTileData());

        const QString fname = fileName(vol, uuid);

        writeJPEG(tileData, fname);
    }

    void writeJPEG(uint8_t const*const data, const QString& fname) const {
        om::jpeg::write8bit(128, 128, data, fname.toStdString());
    }

    void writeJPEG(OmColorARGB const*const data, const QString& fname) const {
        om::jpeg::writeRGB(128, 128, reinterpret_cast<uint8_t const*const>(data),
                           fname.toStdString());
    }

    template <typename T>
    OmTilePtr getTile(T* vol, const om::globalCoord& coord)
    {
        const OmTileCoord tileCoord(0,
                                    coord,
                                    vol,
                                    freshness(vol),
                                    &vgs_,
                                    XY_VIEW,
                                    CHANNEL);

        OmTilePtr tile;
        OmTileCache::Get(tile, tileCoord, om::BLOCKING);

        return tile;
    }
};

