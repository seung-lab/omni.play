#include "system/omBuildSegmentation.h"
#include "utility/omImageDataIo.h"
#include "project/omProject.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "utility/stringHelpers.h"
#include "system/events/omSegmentEvent.h"
#include "system/omEventManager.h"
#include "volume/omSegmentation.h"

#include <QTextStream>

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
		loadDendrogram();
		
		OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
	}

	if( doBuildMesh ){
		do_build_seg_mesh();
	}

	reset();
}

void OmBuildSegmentation::do_build_seg_image()
{
	QString type = "segmentation data";

	if(!checkSettings(type) ){
		return;
	}

	startTiming(type);

	mSeg->SetSourceFilenamesAndPaths( mFileNamesAndPaths );
	mSeg->BuildVolumeData();

	stopTiming(type);
}

void OmBuildSegmentation::do_build_seg_mesh()
{
	QString type = "segmentation mesh";
	startTiming(type);

	mSeg->BuildMeshData();

	stopTiming(type);
}

void OmBuildSegmentation::loadDendrogram()
{
	QString type = "dendrogram";
	if(!canDoLoadDendrogram()){
		printf("no dendrogram found\n");
		printf("************\n");
		return;
	}
	startTiming(type);
	doLoadDendrogram();
	stopTiming(type);
}

void OmBuildSegmentation::doLoadDendrogram()
{
	QString fname = mFileNamesAndPaths.at(0).filePath();

        OmDataLayer dl;
        OmDataReader * hdf5reader = dl.getReader(fname, true);
	hdf5reader->open();

        OmDataPath fpath;
        fpath.setPathQstr("dend");
        if( !hdf5reader->dataset_exists(fpath)){
                printf("no dendrogram dataset found\n");
                return;
        } 
	Vector3 < int > dSize = hdf5reader->dataset_get_dims(fpath);
	printf("dendrogram is %d x %d\n", dSize.x, dSize.y);
	int dendSize;
	OmDataWrapperPtr dend = hdf5reader->dataset_raw_read(fpath, &dendSize);

        fpath.setPathQstr("dendValues");
        if(!hdf5reader->dataset_exists(fpath)){
                printf("no dendrogram values dataset found\n");
                return;
	} 
      	Vector3 < int > vSize = hdf5reader->dataset_get_dims(fpath);
	int dendValuesSize;
	OmDataWrapperPtr dendValues = hdf5reader->dataset_raw_read(fpath, &dendValuesSize);
	printf("dendrogram values is %d x %d\n", vSize.x, vSize.y);

	assert( 2 == dSize.x );
	assert( 0 == vSize.y );
	assert( vSize.x == dSize.y );

	mSeg->mDendCount = dSize.y;
	mSeg->mDend = dend;
	mSeg->mDendSize = dendSize;
	mSeg->mDendValues = dendValues;
	mSeg->mDendValuesSize = dendValuesSize;
	mSeg->FlushDend();

	hdf5reader->close();
}
