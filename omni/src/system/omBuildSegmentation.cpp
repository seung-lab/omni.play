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
#include "segment/lowLevel/DynamicTreeContainer.h"
#include "segment/omSegmentCache.h"

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

	if( !checkSettings() ){
		return;
	}

	startTiming(type);

	mSeg->SetSourceFilenamesAndPaths( mFileNamesAndPaths );
	mSeg->BuildVolumeData();
	loadDendrogram();
	mSeg->GetSegmentCache()->refreshTree();

	stopTiming(type);
}

void OmBuildSegmentation::do_build_seg_mesh()
{
	const QString type = "segmentation mesh";
	startTiming(type);

	mSeg->BuildMeshData();

	stopTiming(type);
}

void OmBuildSegmentation::loadDendrogram()
{
	const QString type = "dendrogram";
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
	printf("\tdendrogram is %d x %d\n", dSize.x, dSize.y);
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
	printf("\tdendrogram values is %d x %d\n", vSize.x, vSize.y);

	assert( 2 == dSize.x );
	assert( 0 == vSize.y );
	assert( vSize.x == dSize.y );
	
	quint8 * userDisabledEdge = (quint8 *)malloc(sizeof(quint8) * dendValuesSize );
	memset(userDisabledEdge, 0, sizeof(quint8) * dendValuesSize );
	OmDataWrapperPtr edgeDisabledByUser = OmDataWrapperPtr( new OmDataWrapper( userDisabledEdge ) );

	// this is just a temporary object--should be refactored... (purcaro)
	quint8 * edgeJoined = (quint8 *)malloc(sizeof(quint8) * dendValuesSize );
	memset(edgeJoined, 0, sizeof(quint8) * dendValuesSize );
	OmDataWrapperPtr edgeWasJoined = OmDataWrapperPtr( new OmDataWrapper( edgeJoined ) );

	quint8 * edgeForce = (quint8 *)malloc(sizeof(quint8) * dendValuesSize );
	memset(edgeForce, 0, sizeof(quint8) * dendValuesSize );
	OmDataWrapperPtr edgeForceJoin = OmDataWrapperPtr( new OmDataWrapper( edgeForce ) );

	mSeg->mDendCount = dSize.y;
	mSeg->mDend = dend;
	mSeg->mDendSize = dendSize;
	mSeg->mDendValues = dendValues;
	mSeg->mDendValuesSize = dendValuesSize;
	mSeg->mEdgeDisabledByUser = edgeDisabledByUser;
	mSeg->mEdgeForceJoin = edgeForceJoin;
	mSeg->mEdgeWasJoined = edgeWasJoined;

	convertToEdgeList( mSeg->mDend->getQuint32Ptr(), 
			   mSeg->mDendValues->getFloatPtr(), 
			   mSeg->mDendCount );

	mSeg->FlushDend();

	hdf5reader->close();
}

// rewrite child node IDs in MST, converting it to edge list
void OmBuildSegmentation::convertToEdgeList( quint32 * dend, 
					     float * dendValues, 
					     const int numDendRows )
{
	const int maxNumSegs =  mSeg->GetSegmentCache()->getMaxValue() + 1;
	DynamicTreeContainer<OmSegID> * mGraph = new DynamicTreeContainer<OmSegID>( maxNumSegs );
	
	unsigned int childUnknownDepthID;
	unsigned int parentID;
	float threshold;
	
	for(int i = 0; i < numDendRows; ++i) {
                childUnknownDepthID = dend[i];
		parentID = dend[i + numDendRows ];
                threshold = dendValues[i];
		
		DynamicTree<OmSegID> * childRootDT = mGraph->get( childUnknownDepthID )->findRoot();
		childRootDT->join( mGraph->get( parentID ) );

		// set child ID to root value found by graph...
		dend[i] = childRootDT->getKey();
        }

	delete mGraph;
}
