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
	return numCores;
}

void OmLocalPreferences::setNumAllowedWorkerThreads( const int numThreads )
{
	printf("fix me!\n");
}

unsigned int OmLocalPreferences::getRamCacheSize()
{
	if( settingExists( "ram" ) ){
		return readSetting("ram");
	} 
		
	return OmSystemInformation::get_total_system_memory_megs() / 3.0;
}

void OmLocalPreferences::setRamCacheSize(unsigned int size)
{
	writeSetting("ram", size);
	OmCacheManager::UpdateCacheSizeFromLocalPrefs();
}

unsigned int OmLocalPreferences::getVRamCacheSize()
{
	if( settingExists( "vram" ) ){
		return readSetting("vram");
	} 
		
	return OmSystemInformation::get_total_system_memory_megs() / 4.0;
}

void OmLocalPreferences::setVRamCacheSize(unsigned int size)
{
	writeSetting("vram", size);
	OmCacheManager::UpdateCacheSizeFromLocalPrefs();
}

void OmLocalPreferences::writeSetting( QString settingName, const int value )
{
	printf("fix me!\n");
}

int OmLocalPreferences::readSetting( QString settingName )
{

	printf("fix me!: defaulting to %d for setting \"%s\"\n", 10000, qPrintable(settingName) );
	return 10000;
}

bool OmLocalPreferences::settingExists( QString settingName )
{
	return Instance()->localPrefFiles->checkIfSettingExists( settingName );
}
