#ifndef OM_LOCAL_CONFIGURATION_H
#define OM_LOCAL_CONFIGURATION_H

#include "common/omCommon.h"
#include "zi/omUtility.h"
#include "utility/localPrefFiles.h"
#include "utility/omSystemInformation.h"
#include "system/cache/omCacheManager.h"
#include "common/omDebug.h"

class OmLocalPreferences : private om::singletonBase<OmLocalPreferences> {
public:

// max number of mesh worker threads
	static int32_t numAllowedWorkerThreads()
	{
		const int numCoresRaw = OmSystemInformation::get_num_cores();
		int numCores = numCoresRaw - 1;
		if(numCoresRaw < 2){
			numCores = 2;
		}

		return LocalPrefFiles::readSettingInt("numThreads", numCores);
	}
	static void setNumAllowedWorkerThreads(const int32_t numThreads)
	{
		LocalPrefFiles::writeSettingInt("numThreads", numThreads);
	}

// RAM size
	static uint32_t getRamCacheSizeMB()
	{
		const unsigned int defaultRet =
			OmSystemInformation::get_total_system_memory_megs() / 3;
		return LocalPrefFiles::readSettingUInt("ram", defaultRet);
	}
	static void setRamCacheSizeMB(const uint32_t size)
	{
		LocalPrefFiles::writeSettingUInt("ram", size);
		OmCacheManager::UpdateCacheSizeFromLocalPrefs();
	}

// VRAM size
	static uint32_t getVRamCacheSizeMB()
	{
		const unsigned int defaultRet =
			OmSystemInformation::get_total_system_memory_megs() / 4;
		return LocalPrefFiles::readSettingUInt("vram", defaultRet);
	}
	static void setVRamCacheSizeMB(const uint32_t size)
	{
		LocalPrefFiles::writeSettingUInt("vram", size);
		OmCacheManager::UpdateCacheSizeFromLocalPrefs();
	}

// recently-used files
	static QStringList getRecentlyUsedFilesNames()
	{
		QStringList empty;
		return LocalPrefFiles::readSettingQStringList("recentlyOpenedFiles", empty);
	}
	static void setRecentlyUsedFilesNames(const QStringList& values)
	{
		LocalPrefFiles::writeSettingQStringList("recentlyOpenedFiles", values);
	}

// scratch space
	static QString getScratchPath()
	{
		QString defaultPath = "/tmp";
		return LocalPrefFiles::readSettingQString("sratchPath", defaultPath);
	}
	static void setScratchPath(const QString& scratchPath)
	{
		LocalPrefFiles::writeSettingQString("sratchPath", scratchPath);
	}

private:
	OmLocalPreferences(){}
	~OmLocalPreferences(){}

	friend class zi::singleton<OmLocalPreferences>;
};

#endif
