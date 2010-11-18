#ifndef OM_BUILD_SEGMENTATION_H
#define OM_BUILD_SEGMENTATION_H

#include "actions/omActions.hpp"
#include "project/omProject.h"
#include "segment/io/omMST.h"
#include "segment/omSegmentCache.h"
#include "system/omBuildVolumes.h"
#include "system/omEvents.h"
#include "utility/omThreadPool.hpp"
#include "volume/build/omVolumeBuilder.hpp"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "zi/omThreads.h"
#include "utility/dataWrappers.h"

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
			do_build_seg_mesh();
		} else {
			threadPool_.addTaskBack(zi::run_fn(zi::bind(&OmBuildSegmentation::do_build_seg_mesh, this)));
		}
	}

	void buildBlankVolume()
	{
		printf("assuming channel 1\n");
		assert(OmProject::IsChannelValid(1));
		OmChannel& chann = OmProject::GetChannel(1);

		seg_.BuildBlankVolume(chann.MipLevelDataDimensions(0));
		seg_.loadVolData();
		seg_.GetSegmentCache()->refreshTree();

		printf("allocated blank volume\n");
	}

	void loadDendrogram()
	{
		const QString type = "dendrogram";
		if(!canDoLoadDendrogram()){
			printf("no dendrogram found\n");
			printf("************\n");
			return;
		}

		OmTimer build_timer;
		startTiming(type, build_timer);

		const QString fname = mFileNamesAndPaths.front().filePath();
		seg_.mst_->import(fname.toStdString());

		stopTimingAndSave(type, build_timer);

		printf("Segmentation MST load COMPLETELY done\n");
		printf("************************\n");
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

		OmVolumeBuilder<OmSegmentation> builder(&seg_);
		builder.SetSourceFilenamesAndPaths( mFileNamesAndPaths );
		OmDataPath path("main");
		builder.Build(path);

		stopTimingAndSave(type, build_timer);

		loadDendrogram();
		OmActions::ChangeMSTthreshold(seg_.GetID(), 0.5);

		OmEvents::SegmentModified();

		printf("Segmentation image COMPLETELY done\n");
		printf("************************\n");
	}

	void do_build_seg_mesh()
	{
		const QString type = "segmentation mesh";

		OmTimer build_timer;
		startTiming(type, build_timer);

		seg_.Mesh();

		stopTimingAndSave(type, build_timer);
	}
};

#endif
