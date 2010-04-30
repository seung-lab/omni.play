#include "omBuildVolumes.h"
#include "utility/omImageDataIo.h"
#include "project/omProject.h"
#include "utility/omDataLayer.h"
#include "utility/omDataReader.h"
#include "utility/stringHelpers.h"

#include <QTextStream>

OmBuildVolumes::OmBuildVolumes(OmSegmentation * seg)
{
	mSeg = seg;
}

OmBuildVolumes::OmBuildVolumes(OmChannel * chan)
{
	mChann = chan;
}

void OmBuildVolumes::addFileNameAndPath( QString fnp )
{
	mFileNamesAndPaths << fnp;
}

void OmBuildVolumes::setFileNamesAndPaths( QFileInfoList fileNamesAndPaths )
{
	mFileNamesAndPaths = fileNamesAndPaths;
}

void OmBuildVolumes::buildAndMeshSegmentation()
{
	build_seg_image();
	build_seg_mesh();
}

bool OmBuildVolumes::checkSettingsAndTime(QString type )
{
	if( mFileNamesAndPaths.empty() ) {
		printf("\tError: can't build: no files selected\n");
		return false;
	}

	if( ! OmImageDataIo::are_file_names_valid(mFileNamesAndPaths)){
		printf("\tError: file list contains invalid files\n");
		return false;
	}

	printf("starting %s build...\n", qPrintable(type));
	time(&time_start);
	return true;
}

void OmBuildVolumes::stopTiming(QString type)
{
	time(&time_end);
	time_dif = difftime(time_end, time_start);

	OmProject::Save();
	printf("\tdone: %s build performed in (%.2lf secs)\n", qPrintable(type), time_dif );
}

void OmBuildVolumes::build_seg_image()
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

void OmBuildVolumes::build_seg_mesh()
{
	QString type = "segmentation mesh";
	if(!checkSettingsAndTime(type) ){
		return;
	}

	mSeg->BuildMeshData();

	stopTiming(type);
}

void OmBuildVolumes::build_channel()
{
	QString type = "channel";
	if(!checkSettingsAndTime(type) ){
		return;
	}

	mChann->SetSourceFilenamesAndPaths( mFileNamesAndPaths );
	mChann->BuildVolumeData();

	stopTiming(type);
}

QString OmBuildVolumes::getSelectedHDF5file()
{
	if( mFileNamesAndPaths.size() != 1){
		return "";
	}

	QString fname = mFileNamesAndPaths.at(0).filePath();

	if( fname.endsWith(".h5")  ||
	    fname.endsWith(".hdf5")){ 
		return fname;
	}

	return "";
}
	
void OmBuildVolumes::loadDendrogram()
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
