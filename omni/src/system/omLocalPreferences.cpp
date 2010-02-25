#include "omLocalPreferences.h"
#include "stddef.h"
#include <stdio.h>
#include "utility/omSystemInformation.h"
#include "system/omCacheManager.h"

OmLocalPreferences *OmLocalPreferences::mspInstance = 0;

OmLocalPreferences::OmLocalPreferences()
{
}

OmLocalPreferences::~OmLocalPreferences()
{
}

OmLocalPreferences *OmLocalPreferences::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmLocalPreferences;
		mspInstance->localPrefFiles = new LocalPrefFiles();
	}	
	return mspInstance;
}

void OmLocalPreferences::Delete()
{
	if (mspInstance) {
		delete mspInstance;
	}
	mspInstance = NULL;
}

int OmLocalPreferences::get_num_cores()
{
	return (int)OmSystemInformation::get_num_cores();
}

int OmLocalPreferences::numAllowedWorkerThreads()
{
	const int numCoresRaw = get_num_cores();
	int numCores = numCoresRaw - 1;
	if( 1 == numCoresRaw ){
		numCores = 1;
	}

	return readSettingInt( "numThreads", numCores );
}

void OmLocalPreferences::setNumAllowedWorkerThreads( const int numThreads )
{
	writeSettingInt( "numThreads", numThreads );
}

unsigned int OmLocalPreferences::getRamCacheSize()
{
	const unsigned int defaultRet = OmSystemInformation::get_total_system_memory_megs() / 3.0;
	return readSettingUInt( "ram", defaultRet );
}

void OmLocalPreferences::setRamCacheSize(unsigned int size)
{
	writeSettingUInt("ram", size);
	OmCacheManager::UpdateCacheSizeFromLocalPrefs();
}

unsigned int OmLocalPreferences::getVRamCacheSize()
{
	const unsigned int defaultRet = OmSystemInformation::get_total_system_memory_megs() / 4.0;
	return readSettingUInt( "vram", defaultRet );
}

void OmLocalPreferences::setVRamCacheSize(const unsigned int size)
{
	writeSettingUInt("vram", size);
	OmCacheManager::UpdateCacheSizeFromLocalPrefs();
}

void OmLocalPreferences::writeSettingUInt( QString setting, const unsigned int value )
{
	Instance()->localPrefFiles->writeSettingUInt( setting, value );
}

void OmLocalPreferences::writeSettingInt( QString setting, const int value )
{
	Instance()->localPrefFiles->writeSettingInt( setting, value );
}

unsigned int OmLocalPreferences::readSettingUInt( QString setting, const unsigned int defaultRet )
{
	if( !settingExists( setting ) ){
		return defaultRet;
	} 

	try{
		return Instance()->localPrefFiles->readSettingUInt( setting  );
	} catch (...) {
		return defaultRet;
	}
}

int OmLocalPreferences::readSettingInt( QString setting, const int defaultRet )
{
	if( !settingExists( setting ) ){
		return defaultRet;
	} 

	try{
		return Instance()->localPrefFiles->readSettingInt( setting  );
	} catch (...) {
		return defaultRet;
	}
}

bool OmLocalPreferences::settingExists( QString setting )
{
	return Instance()->localPrefFiles->checkIfSettingExists( setting );
}

QStringList OmLocalPreferences::readSettingQStringList( QString setting, QStringList defaultRet )
{
	if( !settingExists( setting ) ){
		return defaultRet;
	} 

	try{
		return Instance()->localPrefFiles->readSettingQStringList( setting  );
	} catch (...) {
		return defaultRet;
	}
}

QStringList OmLocalPreferences::getRecentlyUsedFilesNames()
{
	QStringList empty;
	return readSettingQStringList( "recentlyOpenedFiles", empty );
}

void OmLocalPreferences::setRecentlyUsedFilesNames( QStringList values)
{
	Instance()->localPrefFiles->writeSettingQStringList( "recentlyOpenedFiles", values );
}
