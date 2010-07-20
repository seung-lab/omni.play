#ifndef OM_BUILD_VOLUMES_H
#define OM_BUILD_VOLUMES_H

#include "utility/omTimer.h"

#include <QFileInfoList>
#include <QString>
#include <QThread>

class OmBuildVolumes
{
 public:
	OmBuildVolumes();
	void setFileNamesAndPaths( QFileInfoList fileNamesAndPaths );
	void addFileNameAndPath( QString fnp );

 protected:
	bool checkSettings();
	void startTiming(QString type);
	void stopTiming(QString type);


	bool canDoLoadDendrogram();

	QFileInfoList mFileNamesAndPaths;

	void readImages();

 private:
	OmTimer build_timer;

	bool are_file_names_valid();
};

#endif
