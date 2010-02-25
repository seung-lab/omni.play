#ifndef LOCAL_PREF_FILES_H
#define LOCAL_PREF_FILES_H

#include <QDir>

class LocalPrefFiles 
{
 public:
	LocalPrefFiles();
	bool checkIfSettingExists( QString setting );
	unsigned int readSettingUInt( QString setting );

 private:
	QDir prefFolder;
	void setupPrefFolder();
	QString getFileName( QString setting );
	QStringList readFile( QString setting );
};

#endif
