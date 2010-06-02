#ifndef OM_BUILD_VOLUMES_H
#define OM_BUILD_VOLUMES_H

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
	bool checkSettings(QString type);
	void startTiming(QString type);
	void stopTiming(QString type);


	bool canDoLoadDendrogram();

	QFileInfoList mFileNamesAndPaths;

	void readImages();

 private:
	time_t time_start;
	time_t time_end;
	double time_dif;

	bool are_file_names_valid();
};

#endif
