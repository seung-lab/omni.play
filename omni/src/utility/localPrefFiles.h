#ifndef LOCAL_PREF_FILES_H
#define LOCAL_PREF_FILES_H

#include <QDir>

class LocalPrefFiles 
{
 public:
	LocalPrefFiles();
	bool checkIfSettingExists( QString setting );
	int readSettingInt( QString setting );
	unsigned int readSettingUInt( QString setting );
	void writeSettingInt( QString setting, const int value );
	void writeSettingUInt( QString setting, const unsigned int value );

 private:
	QDir prefFolder;
	void setupPrefFolder();
	QString getFileName( QString setting );
	QStringList readFile( QString setting );
};

#endif
