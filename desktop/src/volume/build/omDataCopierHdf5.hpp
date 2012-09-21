#pragma once

#include "common/omCommon.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "chunks/omChunk.h"
#include "utility/omStringHelpers.h"
#include "threads/omTaskManager.hpp"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "volume/build/omDataCopierBase.hpp"
#include "volume/build/omDataCopierHdf5Task.hpp"

#include <QFileInfoList>

template <typename VOL>
class OmDataCopierHdf5 : public OmDataCopierBase<VOL>{
private:
    VOL *const vol_;
    const OmDataPath path_;
    const om::AffinityGraph aff_;

    Vector3i volSize_;
    OmHdf5* hdf5reader_;
    QString mip0fnp_;

public:
    OmDataCopierHdf5(VOL* vol,
                     const OmDataPath& path,
                     const std::string fnp,
                     const om::AffinityGraph aff)
        : OmDataCopierBase<VOL>(vol)
        , vol_(vol)
        , path_(path)
        , aff_(aff)
    {
        hdf5reader_ = OmHdf5Manager::Get(fnp, true);
        hdf5reader_->open();

        std::cout << "importHDF5: source path is: " << path_ << "\n";

        volSize_ = hdf5reader_->getChunkedDatasetDims(path_, aff_);
        std::cout << "importHDF5: source vol dims: " << volSize_ << "\n";
    }

    ~OmDataCopierHdf5(){
        hdf5reader_->close();
    }

private:
    virtual void doImport()
    {
        allocateData(determineDataType());

        OmTimer timer;
        printf("copying in HDF5 data...\n");

        OmThreadPool threadPool;
        threadPool.start(3);

        om::shared_ptr<std::deque<om::chunkCoord> > coordsPtr =
            vol_->GetMipChunkCoords(0);

        OmSimpleProgress prog(coordsPtr->size(), "HDF5 chunk copy");

        FOR_EACH(iter, *coordsPtr){
            const om::chunkCoord& coord = *iter;

            om::shared_ptr<OmDataCopierHdf5Task<VOL> > task =
                om::make_shared<OmDataCopierHdf5Task<VOL> >(vol_,
                                                               path_,
                                                               aff_,
                                                               volSize_,
                                                               hdf5reader_,
                                                               mip0fnp_,
                                                               coord,
                                                               &prog);
            threadPool.push_back(task);
        }

        threadPool.join();

        printf("\nHDF5 data copy done in %f secs\n", timer.s_elapsed());
    }

    void allocateData(const OmVolDataType type)
    {
        std::vector<om::shared_ptr<QFile> > volFiles =
            OmVolumeAllocater::AllocateData(vol_, type);

        mip0fnp_ = QFileInfo(*volFiles[0]).absoluteFilePath();
    }

    OmVolDataType determineDataType()
    {
        OmDataWrapperPtr data =	hdf5reader_->GetChunkDataType(path_);
        return data->getVolDataType();
    }
};

