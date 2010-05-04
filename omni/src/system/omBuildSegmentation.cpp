#include "system/omBuildSegmentation.h"
#include "utility/omImageDataIo.h"
#include "project/omProject.h"
#include "utility/omDataLayer.h"
#include "utility/omDataReader.h"
#include "utility/stringHelpers.h"

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

	loadDendrogram();

	stopTiming(type);
}

void OmBuildSegmentation::do_build_seg_mesh()
{
	QString type = "segmentation mesh";
	if(!checkSettingsAndTime(type) ){
		return;
	}

	mSeg->BuildMeshData();

	stopTiming(type);
}
	
void OmBuildSegmentation::loadDendrogram()
{
	QString fname = getSelectedHDF5file();

	if( "" == fname ){
		return;
	}
	    
	OmHdf5Path fpath;
	fpath.setPathQstr( "dend" );

	OmDataLayer dl;
 	OmDataReader * hdf5reader = dl.getReader( fname, true, true );

	if( !hdf5reader->dataset_exists( fpath ) ){
		printf("no dendrogram found\n");
		return;
	} else {
		printf("found dendrogram!\n");
	}

	//	Vector3 < int > dSize = hdf5reader->dataset_get_dims( fpath );
	//	printf("dendrogram is %d x %d\n", dSize.x, dSize.y );

	QString dendTextFileName = "/Users/purcaro/Omni/purcaro_testing/d/dend.txt";

	if (!QFile::exists( dendTextFileName )) {
		printf("could not open dend text file \"%s\"\n", qPrintable(dendTextFileName));
		exit(1);
	}
		
	QFile file(dendTextFileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		printf("could not read dend text file \"%s\"\n", qPrintable(dendTextFileName));
		exit(1);
	}

	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine();
		QStringList args = line.split(',');
		const unsigned int childVal =  StringHelpers::getUInt(args[0]);
		const unsigned int parentVal =  StringHelpers::getUInt(args[1]);
		const double threshold = StringHelpers::getDouble( args[2] );
		
		OmSegment* child = mSeg->GetSegmentFromValue(childVal);
		OmSegment* parent = mSeg->GetSegmentFromValue(parentVal);

		parent->Join(child, threshold);
	}
}


