#include "system/omBuildSegmentation.h"
#include "utility/omImageDataIo.h"
#include "project/omProject.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataReader.h"
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

	if(!checkSettingsAndTime(type) ){
		return;
	}

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
	if(!checkSettingsAndTime(type) ){
		return;
	}
	if(!canDoLoadDendrogram()){
		return;
	}

	QString fname = mFileNamesAndPaths.at(0).filePath();

        OmDataLayer dl;
        OmDataReader * hdf5reader = dl.getReader(fname, true);

        OmHdf5Path fpath;
        fpath.setPathQstr("dend");
        if( !hdf5reader->dataset_exists(fpath)){
                printf("no dendrogram found\n");
                return;
        } 
	Vector3 < int > dSize = hdf5reader->dataset_get_dims(fpath);
	printf("dendrogram is %d x %d\n", dSize.x, dSize.y);
	int dendSize;
	quint32 * dend = (quint32 *) hdf5reader->dataset_raw_read(fpath, &dendSize);

        fpath.setPathQstr("dendValues");
        if(!hdf5reader->dataset_exists(fpath)){
                printf("no dendrogram values found\n");
		free(dend);
                return;
	} 
      	Vector3 < int > vSize = hdf5reader->dataset_get_dims(fpath);
	int dendValuesSize;
	float * dendValues = (float *) hdf5reader->dataset_raw_read(fpath, &dendValuesSize);
	printf("dendrogram values is %d x %d\n", dSize.x, dSize.y);

	mSeg->mDendCount = dSize.y;
	mSeg->mDend = dend;
	mSeg->mDendSize = dendSize;
	mSeg->mDendValues = dendValues;
	mSeg->mDendValuesSize = dendValuesSize;
	mSeg->FlushDend();

	hdf5reader->close();

	stopTiming(type);
}
