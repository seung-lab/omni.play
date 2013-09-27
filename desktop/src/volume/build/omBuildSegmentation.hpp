#pragma once

#include "actions/omActions.h"
#include "events/omEvents.h"
#include "mesh/omMeshManagers.hpp"
#include "project/omProject.h"
#include "segment/io/omMST.h"
#include "segment/omSegments.h"
#include "threads/omTaskManager.hpp"
#include "utility/dataWrappers.h"
#include "volume/build/omBuildVolumes.hpp"
#include "volume/build/omVolumeBuilder.hpp"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

class OmBuildSegmentation : public OmBuildVolumes {
private:
    SegmentationDataWrapper sdw_;
    OmSegmentation& seg_;

    typedef std::shared_ptr<om::gui::progress> prog_t;

public:
    OmBuildSegmentation()
        : OmBuildVolumes()
        , seg_(sdw_.Create())
    {}

    OmBuildSegmentation(const SegmentationDataWrapper& sdw)
        : OmBuildVolumes()
        , sdw_(sdw)
        , seg_(sdw_.GetSegmentation())
    {}

    virtual ~OmBuildSegmentation(){
        printf("OmBuildSegmentation done!\n");
    }

    SegmentationDataWrapper& GetDataWrapper(){
        return sdw_;
    }

    void BuildAndMeshSegmentation(){
        do_build_seg_image_and_mesh(prog_t());
    }

    void BuildAndMeshSegmentation(prog_t p){
        do_build_seg_image_and_mesh(p);
    }

    void BuildImage(){
        do_build_seg_image();
    }

    void BuildMesh(){
        do_build_seg_mesh(prog_t());
    }

    void BuildMesh(prog_t p){
        do_build_seg_mesh(p);
    }

    void BuildBlankVolume()
    {
        printf("assuming channel 1\n");
        ChannelDataWrapper cdw(1);
        if(!cdw.IsValidWrapper()){
            throw om::IoException("no channel 1");
        }

        OmChannel& chann = cdw.GetChannel();

        seg_.BuildBlankVolume(chann.Coords().MipLevelDataDimensions(0));
        seg_.LoadVolData();
        seg_.Segments()->refreshTree();

        OmActions::Save();

        printf("allocated blank volume\n");
    }

    void LoadDendrogram(){
        throw om::IoException("not implemented");
    }

private:
    void do_build_seg_image_and_mesh(prog_t p)
    {
        do_build_seg_image();
        do_build_seg_mesh(p);
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

    void do_build_seg_mesh(prog_t p)
    {
        const QString type = "segmentation mesh (threshold 1)";

        OmTimer build_timer;
        startTiming(type, build_timer);

        if(p){
            seg_.MeshManagers()->FullMesh(1, p);

        } else {
            seg_.MeshManagers()->FullMesh(1);
            // seg_.MeshManagers()->FullMesh(.9);
            // seg_.MeshManagers()->FullMesh(.8);
        }

        stopTimingAndSave(type, build_timer);
    }
};

