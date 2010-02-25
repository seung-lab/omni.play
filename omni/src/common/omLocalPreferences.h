#ifndef OM_LOCAL_CONFIGURATION_H
#define OM_LOCAL_CONFIGURATION_H

#include <QString>

class OmLocalPreferences {

public:
	static OmLocalPreferences* Instance();
	static void Delete();

	static int get_num_cores();
	static int numAllowedWorkerThreads();
	static void setNumAllowedWorkerThreads(int);

	static int getRamCacheSize();
	static void setRamCacheSize(const int);
	static int getVRamCacheSize();
	static void setVRamCacheSize(const int);

protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmLocalPreferences();
	~OmLocalPreferences();
	OmLocalPreferences(const OmLocalPreferences&);
	OmLocalPreferences& operator= (const OmLocalPreferences&);

private:
	//singleton
	static OmLocalPreferences* mspInstance;

	static void writeSetting( QString settingName, const int value );
	static int readSetting( QString settingName );

};

#endif
