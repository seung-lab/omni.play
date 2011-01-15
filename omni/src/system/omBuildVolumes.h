#ifndef OM_BUILD_VOLUMES_H
#define OM_BUILD_VOLUMES_H

#include "utility/omTimer.h"
#include "common/omDebug.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omIDataReader.h"
#include "project/omProject.h"
#include "actions/omActions.hpp"
#include "utility/omStringHelpers.h"

#include <QTextStream>
#include <QImage>
#include <QFileInfoList>
#include <QString>
#include <QThread>

class OmBuildVolumes {
public:
	OmBuildVolumes(){}
	virtual ~OmBuildVolumes(){}

	void setFileNamesAndPaths(const QFileInfoList& fileNamesAndPaths){
		mFileNamesAndPaths = fileNamesAndPaths.toVector().toStdVector();
	}

	void addFileNameAndPath(const QString& fnp){
		mFileNamesAndPaths.push_back(QFileInfo(fnp));;
	}

protected:
	std::vector<QFileInfo> mFileNamesAndPaths;

	bool checkSettings()
	{
		if( !are_file_names_valid()){
			throw OmArgException("Error: file list contains invalid files");
		}

		return true;
	}

	bool canDoLoadDendrogram()
	{
		if( mFileNamesAndPaths.size() != 1){
			return false;
		}

		QString fname = mFileNamesAndPaths.front().filePath();

		if( fname.endsWith(".h5")  ||
			fname.endsWith(".hdf5")){
			return true;
		}

		return false;
	}

	void readImages()
	{
		foreach(const QFileInfo& finfo, mFileNamesAndPaths ){
			QImage image( finfo.filePath() );
			printf("%s: (%dx%d), %d bytes total; depth %d\n", qPrintable( finfo.filePath() ),
				   image.width(), image.height(), image.byteCount(), image.depth() );
		}
	}

	void startTiming(const QString & type, OmTimer & timer)
	{
		printf("starting %s build...\n", qPrintable(type));
		timer.start();
	}


	void stopTimingAndSave(const QString & type, OmTimer & timer)
	{
		OmActions::Save();
		const double time = timer.s_elapsed();
		printf("done: %s build performed in (%.6f secs)\n",
			   qPrintable(type), time);
		printf("************\n");
	}

private:
	bool are_file_names_valid()
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
};

#endif
