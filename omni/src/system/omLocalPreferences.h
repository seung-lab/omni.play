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

		return LocalPrefFiles::readSettingNumber<int32_t>("numThreads", numCores);
	}
	static void setNumAllowedWorkerThreads(const int32_t numThreads)
	{
		LocalPrefFiles::writeSettingNumber<int32_t>("numThreads", numThreads);
	}

// RAM size
	static uint32_t getRamCacheSizeMB()
	{
		const unsigned int defaultRet =
			OmSystemInformation::get_total_system_memory_megs() / 3;
		return LocalPrefFiles::readSettingNumber<uint32_t>("ram", defaultRet);
	}
	static void setRamCacheSizeMB(const uint32_t size)
	{
		LocalPrefFiles::writeSettingNumber<uint32_t>("ram", size);
		OmCacheManager::UpdateCacheSizeFromLocalPrefs();
	}

// VRAM size
	static uint32_t getVRamCacheSizeMB()
	{
		const unsigned int defaultRet =
			OmSystemInformation::get_total_system_memory_megs() / 4;
		return LocalPrefFiles::readSettingNumber<uint32_t>("vram", defaultRet);
	}
	static void setVRamCacheSizeMB(const uint32_t size)
	{
		LocalPrefFiles::writeSettingNumber<uint32_t>("vram", size);
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

// jump to next segment
	static bool GetShouldJumpToNextSegmentAfterValidate()
	{
		const bool defaultVal = true;
		return LocalPrefFiles::readSettingBool("shouldJumpToNextSegmentAfterValidate", defaultVal);
	}
	static void SetShouldJumpToNextSegmentAfterValidate(const bool val)
	{
		LocalPrefFiles::writeSettingBool("shouldJumpToNextSegmentAfterValidate", val);
	}

// View2d crosshairs
	static bool GetShowView2dCrosshairs()
	{
		const bool defaultVal = true;
		return LocalPrefFiles::readSettingBool("ShowView2dCrosshairs", defaultVal);
	}
	static void SetShowView2dCrosshairs(const bool val)
	{
		LocalPrefFiles::writeSettingBool("ShowView2dCrosshairs", val);
	}

	static int View2dCrosshairHoleSize()
	{
		const int defaultVal = 10;
		return LocalPrefFiles::readSettingNumber<int32_t>("View2dCrosshairHoleSize", defaultVal);
	}
	static void SetView2dCrosshairHoleSize(const int val)
	{
		LocalPrefFiles::writeSettingNumber<int32_t>("View2dCrosshairHoleSize", val);
	}

private:
	OmLocalPreferences(){}
	~OmLocalPreferences(){}

	friend class zi::singleton<OmLocalPreferences>;
};

#endif
