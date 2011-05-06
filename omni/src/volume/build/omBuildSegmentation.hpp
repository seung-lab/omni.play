#ifndef OM_BUILD_SEGMENTATION_H
#define OM_BUILD_SEGMENTATION_H

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
            do_build_seg_image_and_mesh();
    }

    void BuildImage(){
            do_build_seg_image();
	}

    void BuildMesh(){
		do_build_seg_mesh();
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
        do_build_seg_mesh();
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

    void do_build_seg_mesh()
    {
        const QString type = "segmentation mesh (threshold 1)";

        OmTimer build_timer;
        startTiming(type, build_timer);

        seg_.MeshManagers()->FullMesh(1);
// seg_.MeshManagers()->FullMesh(.9);
// seg_.MeshManagers()->FullMesh(.8);

        stopTimingAndSave(type, build_timer);
    }
};

#endif
