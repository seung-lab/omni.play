#ifndef LOCAL_PREF_FILES_H
#define LOCAL_PREF_FILES_H

#include <QDir>

class LocalPrefFiles 
{
 public:
	LocalPrefFiles();
	bool checkIfSettingExists( QString setting );

	bool readSettingBool( QString setting );
	void writeSettingBool( QString setting, const bool value );

	int readSettingInt( QString setting );
	void writeSettingInt( QString setting, const int value );

	unsigned int readSettingUInt( QString setting );
	void writeSettingUInt( QString setting, const unsigned int value );

	QStringList readSettingQStringList( QString setting );
	void writeSettingQStringList( QString setting, QStringList values );

	QString readSettingQString( QString setting );
	void writeSettingQString( QString setting, QString value );
 private:
	QDir prefFolder;
	void setupPrefFolder();
	QString getFileName( QString setting );
	QStringList readFile( QString setting );
};

#endif
