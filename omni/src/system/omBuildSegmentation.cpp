#include "segment/io/omMST.h"
#include "system/omEvents.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/omBuildSegmentation.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "actions/omActions.hpp"

OmBuildSegmentation::OmBuildSegmentation(OmSegmentation * seg)
{
	mSeg = seg;
	reset();
}

void OmBuildSegmentation::reset()
{
	doBuildImage = false;
	doBuildMesh  = false;
}

void OmBuildSegmentation::buildAndMeshSegmentation()
{
	doBuildImage = true;
	doBuildMesh  = true;
	start();
}

void OmBuildSegmentation::build_seg_image()
{
	doBuildImage = true;
	start();
}

void OmBuildSegmentation::build_seg_mesh()
{
	doBuildMesh  = true;
	start();
}

void OmBuildSegmentation::run()
{
	if( doBuildImage ){
		do_build_seg_image();
		OmEvents::SegmentModified();
	}

	if( doBuildMesh ){
		do_build_seg_mesh();
	}

	printf("Segmentation build COMPLETELY done\n");
	printf("************************\n");

	reset();
}

void OmBuildSegmentation::do_build_seg_image()
{
	const QString type = "segmentation data";

	if( !checkSettings() ){
		return;
	}

	OmTimer build_timer;
	startTiming(type, build_timer);

	mSeg->SetSourceFilenamesAndPaths( mFileNamesAndPaths );
	mSeg->BuildVolumeData();

	stopTimingAndSave(type, build_timer);

	loadDendrogram();
	OmActions::ChangeMSTthreshold(mSeg->GetID(), 0.5);
}

void OmBuildSegmentation::do_build_seg_mesh()
{
	const QString type = "segmentation mesh";

	OmTimer build_timer;
	startTiming(type, build_timer);

	mSeg->Mesh();

	stopTimingAndSave(type, build_timer);
}

void OmBuildSegmentation::loadDendrogram()
{
	const QString type = "dendrogram";
	if(!canDoLoadDendrogram()){
		printf("no dendrogram found\n");
		printf("************\n");
		return;
	}

	OmTimer build_timer;
	startTiming(type, build_timer);

	const QString fname = mFileNamesAndPaths.at(0).filePath();
	mSeg->mst_->import(fname.toStdString());

	stopTimingAndSave(type, build_timer);
}

void OmBuildSegmentation::buildBlankVolume()
{
	printf("assuming channel 1\n");
	assert(OmProject::IsChannelValid(1));
	OmChannel& chann = OmProject::GetChannel(1);

	mSeg->BuildBlankVolume(chann.MipLevelDataDimensions(0));
	mSeg->loadVolData();
	mSeg->GetSegmentCache()->refreshTree();

	printf("allocated blank volume\n");
}
