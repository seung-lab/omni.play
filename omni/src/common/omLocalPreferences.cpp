#include "omLocalPreferences.h"
#include "stddef.h"
#include <stdio.h>
#include "utility/omNumCores.h"

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
	return (int)OmNumCores::get_num_cores();
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

int OmLocalPreferences::getRamCacheSize()
{
	return readSetting("ram");
}

void OmLocalPreferences::setRamCacheSize(int size)
{
	writeSetting("ram", size);
}

int OmLocalPreferences::getVRamCacheSize()
{
	return readSetting("vram");
}

void OmLocalPreferences::setVRamCacheSize(int size)
{
	writeSetting("vram", size);
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
