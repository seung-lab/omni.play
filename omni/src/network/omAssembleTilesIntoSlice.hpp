#ifndef OM_ASSEMBLE_TILES_INTO_SLICE_HPP
#define OM_ASSEMBLE_TILES_INTO_SLICE_HPP

#include "common/omDebug.h"
#include "system/cache/omCacheManager.h"
#include "tiles/cache/omTileCache.h"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "utility/dataWrappers.h"
#include "utility/omThreadPool.hpp"
#include "utility/omTimer.hpp"
#include "utility/omUUID.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omMipVolume.h"
#include "zi/omMutex.h"

#include <QPainter>

class OmAssembleTilesIntoSlice {
private:
    OmViewGroupState vgs_;
    const ChannelDataWrapper cdw_;
    const SegmentationDataWrapper sdw_;

    OmThreadPool pool_;

    std::map<int, std::string> sliceNumToFileUUID_;
    zi::spinlock mapLock_;

public:
    OmAssembleTilesIntoSlice()
        : vgs_(NULL)
        , cdw_(1)
        , sdw_(1)
    {
        pool_.start();
    }

    QString MakeImgFiles(const int sliceNum){
        return QString::fromStdString(makeImgFiles(sliceNum));
    }

private:

    std::string makeImgFiles(const int sliceNum)
    {
        const OmUUID uuid;

        {
            zi::guard g(mapLock_);
            if(sliceNumToFileUUID_.count(sliceNum)){
                return sliceNumToFileUUID_[sliceNum];
            }
        }

        {
            OmTimer timer;

            makeImgFile(sliceNum, uuid, cdw_);
            makeImgFile(sliceNum, uuid, sdw_);

            timer.Print("\tdone making image pair");
        }

        {
            zi::guard g(mapLock_);
            sliceNumToFileUUID_[sliceNum] = uuid.Str();
        }

        return uuid.Str();
    }

    template <typename T>
    QString fileName(T* vol, const OmUUID& uuid){
        return doMakeFileName(vol->GetNameHyphen(), uuid);
    }

    QString doMakeFileName(const std::string volNameHyphen, const OmUUID& uuid)
    {
        return QString::fromStdString(
            "/var/www/temp_omni_imgs/" + volNameHyphen + "/" +
            uuid.Str() + ".png");
    }

    QImage::Format getQTimageFormat(OmChannel*) const {
        return QImage::Format_Indexed8;
    }

    QImage::Format getQTimageFormat(OmSegmentation*) const {
        return QImage::Format_ARGB32_Premultiplied;
    }

    uint32_t freshness(OmChannel*){
        return 0;
    }

    uint32_t freshness(OmSegmentation*){
        return OmCacheManager::GetFreshness();
    }

    void processImage(OmChannel*, QImage& img)
    {
        for(int i = 0; i < 256; ++i){
            img.setColor(i, qRgb(i,i,i));
        }
    }

    void processImage(OmSegmentation*, QImage& img){
        img.colorTable(); // fixes colors
    }

    template <typename T, typename U>
    struct CopyTileTask {
        T* vol;
        int x;
        int y;
        int sliceNum;
        DataCoord dataCoord;
        U* data;
        int width;
        zi::semaphore* semaphore;
    };

    template <typename T, typename U>
    void getAndCopyTile(boost::shared_ptr<CopyTileTask<T,U> > params)
    {
        OmTilePtr tile = getTile(params->vol, params->dataCoord);
        const OmTextureIDPtr& texture = tile->GetTexture();
        U* tileData = reinterpret_cast<U*>(texture->GetTileData());

        int j = 0;
        for(int i = params->y; i < params->y+128; ++i)
        {
            U* dst = &(params->data[ i*params->width + params->x ]);
            memcpy(dst, &tileData[j], sizeof(U)*128);
            j += 128;
        }

        params->semaphore->release(1);
    }

    void makeImgFile(const int sliceNum, const OmUUID& uuid,
                     const ChannelDataWrapper& wrapper)
    {
        if(wrapper.IsValidWrapper()){
            makeImgFileValid<OmChannel, uint8_t>(sliceNum, uuid,
                                                 wrapper.GetChannelPtr());
        } else {
            makeBlankImage("channel-1", uuid);
        }
    }

    void makeImgFile(const int sliceNum, const OmUUID& uuid,
                     const SegmentationDataWrapper& wrapper)
    {
        if(wrapper.IsValidWrapper()){
            makeImgFileValid<OmSegmentation, OmColorARGB>(sliceNum, uuid,
                                                          wrapper.GetSegmentationPtr());
        } else {
            makeBlankImage("segmentation-1", uuid);
        }
    }

    void makeBlankImage(const QString volName, const OmUUID& uuid)
    {
        const QString fname = doMakeFileName(volName.toStdString(), uuid);

        QImage blank(512, 512, QImage::Format_ARGB32_Premultiplied);
        blank.fill(QColor(Qt::gray).rgb());

        // QPainter p(&blank);
        // p.drawText(25, 256, volName);

        if(!blank.save(fname)){
            throw OmIoException("could not write file", fname);
        }
    }

    template <typename T, typename U>
    void makeImgFileValid(const int sliceNum, const OmUUID& uuid, T* vol)
    {
        const Vector3i roundedUpDataSize = vol->Coords().getDimsRoundedToNearestChunk(0);
        const int width = roundedUpDataSize.x;
        const int height = roundedUpDataSize.y;

        boost::scoped_ptr<U> dataPtr(new U[width * height]);
        U* data = dataPtr.get();

        zi::semaphore semaphore;
        semaphore.set(0);

        int tileCounter = 0;

        for(int x = 0; x < width; x += 128)
        {
            for(int y = 0; y < height; y += 128)
            {
                ++tileCounter;

                const DataCoord dataCoord(x, y, sliceNum);

                boost::shared_ptr<CopyTileTask<T,U> > task =
                    boost::make_shared<CopyTileTask<T,U> >();

                task->vol = vol;
                task->x = x;
                task->y = y;
                task->sliceNum = sliceNum;
                task->dataCoord = dataCoord;
                task->data = data;
                task->width = width;
                task->semaphore = &semaphore;

                pool_.push_back(
                    zi::run_fn(
                        zi::bind(&OmAssembleTilesIntoSlice::getAndCopyTile<T,U>,
                                 this, task)));
            }
        }

        semaphore.acquire(tileCounter);

        QImage slice((uchar*)data, width, height, getQTimageFormat(vol));

        processImage(vol, slice);

        const Vector3i actualDataSize = vol->Coords().GetDataExtent().getMax();
        const QImage cropped = slice.copy(0, 0, actualDataSize.x, actualDataSize.y);

        const QImage scaled = cropped.scaled(512, 512, Qt::KeepAspectRatio);

        const QString fname = fileName(vol, uuid);

        if(!scaled.save(fname)){
            throw OmIoException("could not write file", fname);
        }
    }

    template <typename T>
    OmTilePtr getTile(T* vol, const DataCoord& dataCoord)
    {
        const OmTileCoord tileCoord(0,
                                    dataCoord,
                                    vol,
                                    freshness(vol),
                                    &vgs_,
                                    XY_VIEW,
                                    vol->getVolumeType());

        OmTilePtr tile;
        OmTileCache::doGet(tile, tileCoord, om::BLOCKING);

        return tile;
    }
};

#endif
