#include "omBuildVolumes.h"
#include "utility/omImageDataIo.h"
#include "project/omProject.h"
#include "utility/omDataLayer.h"
#include "utility/omDataReader.h"
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

void OmBuildVolumes::readImages()
{
	foreach( QFileInfo finfo, mFileNamesAndPaths ){
		QImage image( finfo.filePath() );
		printf("%s: (%dx%d), %d bytes total; depth %d\n", qPrintable( finfo.filePath() ),
		       image.width(), image.height(), image.byteCount(), image.depth() );
	}
}
