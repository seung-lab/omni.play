#include "common/omDebug.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omIDataReader.h"
#include "project/omProject.h"
#include "actions/omProjectSaveAction.h"
#include "system/omBuildVolumes.h"
#include "utility/omImageDataIo.h"
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

bool OmBuildVolumes::checkSettings()
{
	if( !are_file_names_valid()){
		printf("\tError: file list contains invalid files\n");
		return false;
	}

	return true;
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

		const QString ext = file.suffix().toLower();
		if( "tif" == ext ||
		    "tiff" == ext ||
		    "jpg" == ext ||
		    "png" == ext ||
		    "h5" == ext ||
		    "hdf5" == ext ){

		} else {
			printf("invalid file: %s\n", qPrintable(file.filePath()) );
			return false;
		}
	}

	return true;
}

void OmBuildVolumes::startTiming(const QString & type, OmTimer & timer)
{
	printf("starting %s build...\n", qPrintable(type));
	timer.start();
}

void OmBuildVolumes::stopTimingAndSave(const QString & type, OmTimer & timer)
{
        (new OmProjectSaveAction())->Run();
	const double time = timer.s_elapsed();
	printf("done: %s build performed in (%.6f secs)\n",
	       qPrintable(type), time);
	printf("************\n");
}
