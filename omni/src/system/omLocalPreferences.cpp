#include "omLocalPreferences.h"
#include "stddef.h"
#include <stdio.h>
#include "utility/omSystemInformation.h"
#include "system/cache/omCacheManager.h"
#include "common/omDebug.h"

int OmLocalPreferences::get_num_cores()
{
	return (int)OmSystemInformation::get_num_cores();
}

/////////////////////////////
// max number of mesh worker threads
int OmLocalPreferences::numAllowedWorkerThreads()
{
	const int numCoresRaw = get_num_cores();
	int numCores = numCoresRaw - 1;
	if( 1 == numCoresRaw ){
		numCores = 1;
	}

	return LocalPrefFiles::readSettingInt( "numThreads", numCores );
}

void OmLocalPreferences::setNumAllowedWorkerThreads( const int numThreads )
{
	LocalPrefFiles::writeSettingInt( "numThreads", numThreads );
}

/////////////////////////////
// RAM size
unsigned int OmLocalPreferences::getRamCacheSizeMB()
{
	const unsigned int defaultRet = OmSystemInformation::get_total_system_memory_megs() / 3;
	return LocalPrefFiles::readSettingUInt( "ram", defaultRet );
}

void OmLocalPreferences::setRamCacheSizeMB(unsigned int size)
{
	LocalPrefFiles::writeSettingUInt("ram", size);
	OmCacheManager::UpdateCacheSizeFromLocalPrefs();
}

/////////////////////////////
// VRAM size
unsigned int OmLocalPreferences::getVRamCacheSizeMB()
{
	const unsigned int defaultRet = OmSystemInformation::get_total_system_memory_megs() / 4;
	return LocalPrefFiles::readSettingUInt( "vram", defaultRet );
}

void OmLocalPreferences::setVRamCacheSizeMB(const unsigned int size)
{
	LocalPrefFiles::writeSettingUInt("vram", size);
	OmCacheManager::UpdateCacheSizeFromLocalPrefs();
}

/////////////////////////////
// recently-used files
QStringList OmLocalPreferences::getRecentlyUsedFilesNames()
{
	QStringList empty;
	return LocalPrefFiles::readSettingQStringList( "recentlyOpenedFiles", empty );
}

void OmLocalPreferences::setRecentlyUsedFilesNames( QStringList values)
{
	LocalPrefFiles::writeSettingQStringList( "recentlyOpenedFiles", values );
}

void OmLocalPreferences::setScratchPath( QString scratchPath )
{
	LocalPrefFiles::writeSettingQString( "sratchPath", scratchPath );
}

QString OmLocalPreferences::getScratchPath( )
{
	QString defaultPath = "/tmp";
	return LocalPrefFiles::readSettingQString( "sratchPath", defaultPath );
}

