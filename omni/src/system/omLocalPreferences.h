#ifndef OM_LOCAL_CONFIGURATION_H
#define OM_LOCAL_CONFIGURATION_H

#include "common/omCommon.h"
#include "zi/omUtility.h"
#include "utility/localPrefFiles.h"

class OmLocalPreferences : private om::singletonBase<OmLocalPreferences> {
public:
	static int get_num_cores();

	static int numAllowedWorkerThreads();
	static void setNumAllowedWorkerThreads(int);

	static unsigned int getRamCacheSizeMB();
	static void setRamCacheSizeMB(const unsigned int);

	static unsigned int getVRamCacheSizeMB();
	static void setVRamCacheSizeMB(const unsigned int);

	static QStringList getRecentlyUsedFilesNames();
	static void setRecentlyUsedFilesNames( QStringList values);

	static QString getScratchPath( );
	static void setScratchPath( QString value);

private:
	OmLocalPreferences(){}
	~OmLocalPreferences(){}

	friend class zi::singleton<OmLocalPreferences>;
};

#endif
