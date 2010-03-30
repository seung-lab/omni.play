#include "buildVolumes.h"
#include "utility/omImageDataIo.h"

BuildVolumes::BuildVolumes(OmSegmentation * seg)
{
	mSeg = seg;
}

BuildVolumes::BuildVolumes(OmChannel * chan)
{
	mChann = chan;
}

void BuildVolumes::addFileNameAndPath( QString fnp )
{
	mFileNamesAndPaths << fnp;
}

void BuildVolumes::setFileNamesAndPaths( QFileInfoList fileNamesAndPaths )
{
	mFileNamesAndPaths = fileNamesAndPaths;
}

void BuildVolumes::buildAndMeshSegmentation()
{
	build_seg_image();
	build_seg_mesh();
}

bool BuildVolumes::checkSettingsAndTime(QString type )
{
	if( mFileNamesAndPaths.empty() ) {
		printf("\tError: can't build: no files selected\n");
		return false;
	}

	if( !are_file_names_valid(mFileNamesAndPaths)){
		printf("\tError: file list contains invalid files\n");
		return false;
	}

	printf("starting %s build...\n", qPrintable(type));
	time(&time_start);
	return true;
}

void BuildVolumes::stopTiming(QString type)
{
	time(&time_end);
	time_dif = difftime(time_end, time_start);
	printf("\t%s build performed in (%.2lf secs)\n", qPrintable(type), time_dif );
}

void BuildVolumes::build_seg_image()
{
	QString type = "segmentation data";

	if(!checkSettingsAndTime(type) ){
		return;
	}

	mSeg->SetSourceFilenamesAndPaths( mFileNamesAndPaths );
	mSeg->BuildVolumeData();

	stopTiming(type);
}

void BuildVolumes::build_seg_mesh()
{
	QString type = "segmentation mesh";
	if(!checkSettingsAndTime(type) ){
		return;
	}

	mSeg->BuildMeshData();

	stopTiming(type);
}

void BuildVolumes::build_channel()
{
	QString type = "channel";
	if(!checkSettingsAndTime(type) ){
		return;
	}

	mChann->SetSourceFilenamesAndPaths( mFileNamesAndPaths );
	mChann->BuildVolumeData();

	stopTiming(type);
}

	
