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
	QFileInfoList mFileNamesAndPaths;

	bool checkSettings();
	bool canDoLoadDendrogram();
	void readImages();

	void startTiming(const QString & type, OmTimer & timer);
	void stopTimingAndSave(const QString & type, OmTimer & timer);

 private:
	bool are_file_names_valid();
};

#endif
