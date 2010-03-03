#include "omLocalPreferences.h"
#include "stddef.h"
#include <stdio.h>
#include "utility/omSystemInformation.h"
#include "system/omCacheManager.h"
#include "system/omEventManager.h"
#include "system/events/omViewEvent.h"

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
		mspInstance->stickyCrosshairMode=NULL;
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

/////////////////////////////
// private data accessors
bool OmLocalPreferences::readSettingBool( QString setting, const bool defaultRet )
{
	if( !settingExists( setting ) ){
		return defaultRet;
	} 

	try{
		return Instance()->localPrefFiles->readSettingBool( setting  );
	} catch (...) {
		return defaultRet;
	}
}

void OmLocalPreferences::writeSettingBool( QString setting, const bool value )
{
	Instance()->localPrefFiles->writeSettingBool( setting, value );
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

void OmLocalPreferences::writeSettingUInt( QString setting, const unsigned int value )
{
	Instance()->localPrefFiles->writeSettingUInt( setting, value );
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

/////////////////////////////
// num cores
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

	return readSettingInt( "numThreads", numCores );
}

void OmLocalPreferences::setNumAllowedWorkerThreads( const int numThreads )
{
	writeSettingInt( "numThreads", numThreads );
}

/////////////////////////////
// RAM size
unsigned int OmLocalPreferences::getRamCacheSizeMB()
{
	const unsigned int defaultRet = OmSystemInformation::get_total_system_memory_megs() / 3.0;
	return readSettingUInt( "ram", defaultRet );
}

void OmLocalPreferences::setRamCacheSizeMB(unsigned int size)
{
	writeSettingUInt("ram", size);
	OmCacheManager::UpdateCacheSizeFromLocalPrefs();
}

/////////////////////////////
// VRAM size
unsigned int OmLocalPreferences::getVRamCacheSizeMB()
{
	const unsigned int defaultRet = OmSystemInformation::get_total_system_memory_megs() / 4.0;
	return readSettingUInt( "vram", defaultRet );
}

void OmLocalPreferences::setVRamCacheSizeMB(const unsigned int size)
{
	writeSettingUInt("vram", size);
	OmCacheManager::UpdateCacheSizeFromLocalPrefs();
}

/////////////////////////////
// sticky cross-hair mode
bool OmLocalPreferences::getStickyCrosshairMode()
{
	if (Instance()->stickyCrosshairMode==NULL){
		int defaultRet = 1;
		Instance()->stickyCrosshairMode = (bool*) malloc(sizeof(bool));
		Instance()->stickyCrosshairMode[0] = (bool) readSettingUInt( "stickyCrosshairMode", defaultRet );
	}
	bool sticky = Instance()->stickyCrosshairMode[0];
	return sticky;		
}

void OmLocalPreferences::setStickyCrosshairMode(bool sticky)
{
	if (Instance()->stickyCrosshairMode==NULL){
		Instance()->stickyCrosshairMode = (bool*) malloc(sizeof(bool));
	} 
	Instance()->stickyCrosshairMode[0] = sticky;
	if (sticky) OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
	writeSettingUInt("stickyCrosshairMode", sticky);
}

/////////////////////////////
// recently-used files
QStringList OmLocalPreferences::getRecentlyUsedFilesNames()
{
	QStringList empty;
	return readSettingQStringList( "recentlyOpenedFiles", empty );
}

void OmLocalPreferences::setRecentlyUsedFilesNames( QStringList values)
{
	Instance()->localPrefFiles->writeSettingQStringList( "recentlyOpenedFiles", values );
}

/////////////////////////////
// where to store temporary meshes
bool OmLocalPreferences::getStoreMeshesInTempFolder()
{
	const bool defaultRet = false;
	return readSettingBool( "storeMeshesInTempFolder", defaultRet );
}

void OmLocalPreferences::setStoreMeshesInTempFolder( const bool value )
{
	writeSettingBool( "storeMeshesInTempFolder", value );
}
