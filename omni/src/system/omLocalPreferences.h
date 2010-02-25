#ifndef OM_LOCAL_CONFIGURATION_H
#define OM_LOCAL_CONFIGURATION_H

#include <QString>
#include "utility/localPrefFiles.h"

class OmLocalPreferences {

public:
	static OmLocalPreferences* Instance();
	static void Delete();

	static int get_num_cores();
	static int numAllowedWorkerThreads();
	static void setNumAllowedWorkerThreads(int);

	static unsigned int getRamCacheSizeMB();
	static void setRamCacheSizeMB(const unsigned int);
	static unsigned int getVRamCacheSizeMB();
	static void setVRamCacheSizeMB(const unsigned int);
	static bool getStickyCrosshairMode();
	static void setStickyCrosshairMode(bool sticky);

	static QStringList getRecentlyUsedFilesNames();
	static void setRecentlyUsedFilesNames( QStringList values);

protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmLocalPreferences();
	~OmLocalPreferences();
	OmLocalPreferences(const OmLocalPreferences&);
	OmLocalPreferences& operator= (const OmLocalPreferences&);

private:
	//singleton
	static OmLocalPreferences* mspInstance;

	static void writeSettingInt( QString setting, const int value );
	static void writeSettingUInt( QString setting, const unsigned int value );

	static unsigned int readSettingUInt( QString setting, const unsigned int defaultRet );
	static int readSettingInt( QString setting, const int defaultRet );
	static QStringList readSettingQStringList( QString setting, QStringList defaultRet );
	
	static bool settingExists( QString setting );

	bool * stickyCrosshairMode;
	LocalPrefFiles* localPrefFiles;
};

#endif
