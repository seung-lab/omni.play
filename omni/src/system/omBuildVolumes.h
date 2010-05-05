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
	bool checkSettingsAndTime(QString type);
	void stopTiming(QString type);

	QString getSelectedHDF5file();

	QFileInfoList mFileNamesAndPaths;

	void readImages();

 private:
	time_t time_start;
	time_t time_end;
	double time_dif;
};

#endif
