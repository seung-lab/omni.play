#ifndef OM_DATA_COPIED_HDF5_TASK_HPP
#define OM_DATA_COPIED_HDF5_TASK_HPP

#include "chunks/omChunk.h"
#include "common/omCommon.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "threads/omTaskManager.hpp"
#include "utility/image/omImage.hpp"
#include "utility/omSimpleProgress.hpp"
#include "volume/omVolumeTypes.hpp"
#include "zi/omThreads.h"

#include <sstream>
#include <QFileInfoList>

template <typename VOL>
class OmDataCopierHdf5Task : public zi::runnable {
private:
    VOL *const vol_;
    const OmDataPath path_;
    const om::AffinityGraph aff_;

    const Vector3i volSize_;
    OmHdf5 *const hdf5reader_;
    const QString mip0fnp_;
    const OmChunkCoord coord_;

    OmSimpleProgress* prog_;

    OmChunk* chunk_;
    bool resizedChunk_;

public:
    OmDataCopierHdf5Task(VOL* vol,
                         const OmDataPath& path,
                         const om::AffinityGraph aff,
                         const Vector3i volSize,
                         OmHdf5 *const hdf5reader,
                         const QString mip0fnp,
                         const OmChunkCoord& coord,
                         OmSimpleProgress* prog)
        : vol_(vol)
        , path_(path)
        , aff_(aff)
        , volSize_(volSize)
        , hdf5reader_(hdf5reader)
        , mip0fnp_(mip0fnp)
        , coord_(coord)
        , prog_(prog)
        , resizedChunk_(false)
    {
        chunk_ = vol_->GetChunk(coord_);
    }

    virtual ~OmDataCopierHdf5Task()
    {}

    void run()
    {
        OmTimer timer;

        copyIntoChunk();

        std::ostringstream stm;
        stm << "copied ";
        if(resizedChunk_){
            stm << "and resized ";
        }
        stm << "HDF5 chunk: " << coord_
            << " in " << timer.s_elapsed() << " secs ";

        prog_->DidOne(stm.str());
    }

private:
    template <typename T>
    OmDataWrapperPtr doResizePartialChunk(OmDataWrapperPtr data,
                                          const DataBbox& chunkExtent,
                                          const DataBbox& dataExtent)
    {
        resizedChunk_ = true;

        const Vector3i dataSize = dataExtent.getUnitDimensions();

        //weirdness w/ hdf5 and/or boost::multi_arry requires flipping x/z
        //TODO: figure out!
        OmImage<T, 3> partialChunk(OmExtents
                                   [dataSize.z]
                                   [dataSize.y]
                                   [dataSize.x],
                                   data->getPtr<T>());

        const Vector3i chunkSize = chunkExtent.getUnitDimensions();

        partialChunk.resize(chunkSize);

        return om::ptrs::Wrap(partialChunk.getMallocCopyOfData());
    }

    OmDataWrapperPtr resizePartialChunk(OmDataWrapperPtr data,
                                        const DataBbox& chunkExtent,
                                        const DataBbox& dataExtent)
    {
        switch(data->getVolDataType().index()){
        case OmVolDataType::INT8:
            return doResizePartialChunk<int8_t>(data, chunkExtent, dataExtent);
        case OmVolDataType::UINT8:
            return doResizePartialChunk<uint8_t>(data, chunkExtent, dataExtent);
        case OmVolDataType::INT32:
            return doResizePartialChunk<int32_t>(data, chunkExtent, dataExtent);
        case OmVolDataType::UINT32:
            return doResizePartialChunk<uint32_t>(data, chunkExtent, dataExtent);
        case OmVolDataType::FLOAT:
            return doResizePartialChunk<float>(data, chunkExtent, dataExtent);
        case OmVolDataType::UNKNOWN:
        default:
            throw OmIoException("unknown data type");
        }
    }

    void copyIntoChunk()
    {
        OmDataWrapperPtr data = getChunkData();

        const uint64_t chunkOffset =
            OmChunkOffset::ComputeChunkPtrOffsetBytes(vol_, coord_);

        QFile file(mip0fnp_);
        if(!file.open(QIODevice::ReadWrite)){
            throw OmIoException("could not open file", mip0fnp_);
        }

        file.seek(chunkOffset);
        file.write(data->getPtr<const char>(), chunk_->Mipping().NumBytes());
    }

    OmDataWrapperPtr getChunkData()
    {
        // get chunk data bbox
        const DataBbox& chunkExtent = chunk_->Mipping().GetExtent();

        const DataBbox volExtent(Vector3i::ZERO,
                                 volSize_.x, volSize_.y, volSize_.z);

        //if data extent contains given extent, read full chunk
        if (volExtent.contains(chunkExtent)) {
            return hdf5reader_->readChunk(path_, chunkExtent, aff_);
        }

        // else, read what we can, and resize

        DataBbox intersect_extent = chunkExtent;
        intersect_extent.intersect(volExtent);

        if(intersect_extent.isEmpty()) {
            throw OmIoException("should not have happened");
        }

        OmDataWrapperPtr partialChunk =
            hdf5reader_->readChunk(path_, intersect_extent, aff_);

        return resizePartialChunk(partialChunk,
                                  chunkExtent,
                                  intersect_extent);
    }
};

#endif
