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

	static unsigned int getRamCacheSize();
	static void setRamCacheSize(const unsigned int);
	static unsigned int getVRamCacheSize();
	static void setVRamCacheSize(const unsigned int);

protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmLocalPreferences();
	~OmLocalPreferences();
	OmLocalPreferences(const OmLocalPreferences&);
	OmLocalPreferences& operator= (const OmLocalPreferences&);

private:
	//singleton
	static OmLocalPreferences* mspInstance;

	static void writeSetting( QString setting, const int value );
	static unsigned int readSettingUInt( QString setting, const unsigned int defaultRet );
	static bool settingExists( QString setting );

	LocalPrefFiles* localPrefFiles;
};

#endif
