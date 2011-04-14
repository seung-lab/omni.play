#ifndef OM_BUILD_SEGMENTATION_H
#define OM_BUILD_SEGMENTATION_H

#include "actions/omActions.h"
#include "mesh/omMipMeshManagers.hpp"
#include "project/omProject.h"
#include "segment/io/omMST.h"
#include "segment/omSegments.h"
#include "events/omEvents.h"
#include "utility/dataWrappers.h"
#include "utility/omThreadPool.hpp"
#include "volume/build/omBuildVolumes.hpp"
#include "volume/build/omVolumeBuilder.hpp"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "zi/omThreads.h"

class OmBuildSegmentation : public OmBuildVolumes {
private:
    SegmentationDataWrapper sdw_;

    OmSegmentation& seg_;
    OmThreadPool threadPool_;

public:
    OmBuildSegmentation()
        : OmBuildVolumes()
        , seg_(sdw_.Create())
    {
        threadPool_.start(1);
    }

    OmBuildSegmentation(const SegmentationDataWrapper& sdw)
        : OmBuildVolumes()
        , sdw_(sdw)
        , seg_(sdw_.GetSegmentation())
    {
        threadPool_.start(1);
    }

    virtual ~OmBuildSegmentation()
    {
        threadPool_.join();
        printf("OmBuildSegmentation done!\n");
    }

    SegmentationDataWrapper& GetDataWrapper(){
        return sdw_;
    }

    void BuildAndMeshSegmentation(const om::Blocking block)
    {
        if(om::BLOCKING == block){
            do_build_seg_image_and_mesh();
        } else {
            threadPool_.addTaskBack(zi::run_fn(zi::bind(&OmBuildSegmentation::do_build_seg_image_and_mesh, this)));
        }
    }

    void BuildImage(const om::Blocking block)
    {
        if(om::BLOCKING == block){
            do_build_seg_image();
        } else {
            threadPool_.addTaskBack(zi::run_fn(zi::bind(&OmBuildSegmentation::do_build_seg_image, this)));
        }
    }

    void BuildMesh(const om::Blocking block)
    {
        if(om::BLOCKING == block){
            do_build_seg_mesh(true);
        } else {
            threadPool_.addTaskBack(zi::run_fn(zi::bind(&OmBuildSegmentation::do_build_seg_mesh, this, true)));
        }
    }

    void BuildMesh(const double threshold)
    {
        do_build_seg_mesh(threshold);
    }

    void buildBlankVolume()
    {
        printf("assuming channel 1\n");
        ChannelDataWrapper cdw(1);
        if(!cdw.IsValidWrapper()){
            throw OmIoException("no channel 1");
        }

        OmChannel& chann = cdw.GetChannel();

        seg_.BuildBlankVolume(chann.Coords().MipLevelDataDimensions(0));
        seg_.loadVolData();
        seg_.Segments()->refreshTree();

        OmActions::Save();

        printf("allocated blank volume\n");
    }

    void loadDendrogram(){
        throw OmIoException("not implemented");
    }

private:
    void do_build_seg_image_and_mesh()
    {
        do_build_seg_image();
        do_build_seg_mesh(false);
    }

    void do_build_seg_image()
    {
        const QString type = "segmentation data";

        if( !checkSettings() ){
            return;
        }

        OmTimer build_timer;
        startTiming(type, build_timer);

        OmVolumeBuilder<OmSegmentation> builder(&seg_,
                                                mFileNamesAndPaths,
                                                "main");
        builder.Build();

        stopTimingAndSave(type, build_timer);

        printf("Segmentation image COMPLETELY done\n");
        printf("************************\n");
    }

    void do_build_seg_mesh(const bool redownsample)
    {
        const QString type = "segmentation mesh";

        OmTimer build_timer;
        startTiming(type, build_timer);

        seg_.MeshManagers()->FullMesh(1, redownsample);
// seg_.MeshManagers()->FullMesh(.9);
// seg_.MeshManagers()->FullMesh(.8);

        stopTimingAndSave(type, build_timer);
    }

    void do_build_seg_mesh(const double threshold, const bool redownsample)
    {
        const QString type = "segmentation mesh";

        OmTimer build_timer;
        startTiming(type, build_timer);

        seg_.MeshManagers()->FullMesh(threshold, redownsample);

        stopTimingAndSave(type, build_timer);
    }
};

#endif
