#include "omBuildVolumes.h"
#include "utility/omImageDataIo.h"
#include "project/omProject.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataReader.h"
#include "utility/stringHelpers.h"

#include <QTextStream>
#include <QImage>

OmBuildVolumes::OmBuildVolumes()
{
}

void OmBuildVolumes::addFileNameAndPath( QString fnp )
{
	mFileNamesAndPaths << fnp;
}

void OmBuildVolumes::setFileNamesAndPaths( QFileInfoList fileNamesAndPaths )
{
	mFileNamesAndPaths = fileNamesAndPaths;
}

bool OmBuildVolumes::checkSettings(QString type )
{
	if( !are_file_names_valid()){
		printf("\tError: file list contains invalid files\n");
		return false;
	}

	return true;
}

void OmBuildVolumes::startTiming(QString type)
{
	printf("starting %s build...\n", qPrintable(type));
	time(&time_start);
}

void OmBuildVolumes::stopTiming(QString type)
{
	time(&time_end);
	time_dif = difftime(time_end, time_start);

	OmProject::Save();
	printf("done: %s build performed in (%.2lf secs)\n", qPrintable(type), time_dif );
	printf("************\n");
}

bool OmBuildVolumes::canDoLoadDendrogram()
{
	if( mFileNamesAndPaths.size() != 1){
		return false;
	}

	QString fname = mFileNamesAndPaths.at(0).filePath();

	if( fname.endsWith(".h5")  ||
	    fname.endsWith(".hdf5")){ 
		return true;
	}

	return false;
}

void OmBuildVolumes::readImages()
{
	foreach( QFileInfo finfo, mFileNamesAndPaths ){
		QImage image( finfo.filePath() );
		printf("%s: (%dx%d), %d bytes total; depth %d\n", qPrintable( finfo.filePath() ),
		       image.width(), image.height(), image.byteCount(), image.depth() );
	}
}

bool OmBuildVolumes::are_file_names_valid()
{
	if( mFileNamesAndPaths.empty() ){
		printf("\tError: can't build: no files selected\n");
		return false;
	}

	foreach( QFileInfo file, mFileNamesAndPaths ){
		if( !file.exists() ){
			printf("file does not exist: %s\n", qPrintable(file.filePath()) );
			return false;
		}

		switch ( OmImageDataIo::om_imagedata_parse_image_type( file.filePath() )){
		case TIFF_TYPE:
		case JPEG_TYPE:
		case PNG_TYPE:
		case VTK_TYPE:
		case HDF5_TYPE:
			break;

		default:
			printf("invalid file: %s\n", qPrintable(file.filePath()) );
			return false;
		}
	}

	return true;
}
