#include "omLocalPreferences.h"
#include "stddef.h"
#include <stdio.h>
#include "utility/omSystemInformation.h"
#include "system/omCacheManager.h"
#include "system/omEventManager.h"
#include "system/events/omViewEvent.h"
#include "common/omDebug.h"

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

void OmLocalPreferences::writeSettingQString (QString setting, QString value)
{
	Instance()->localPrefFiles->writeSettingQString( setting, value );
}

QString OmLocalPreferences::readSettingQString (QString setting, QString defaultRet)
{
        if( !settingExists( setting ) ){
                return defaultRet;
        }

        try{
                return Instance()->localPrefFiles->readSettingQString( setting  );
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
		const unsigned int defaultRet = 1;
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

unsigned int OmLocalPreferences::getDefaultCrosshairValue()
{
	const unsigned int defaultRet = 1;
	Instance()->mCrosshairValue = readSettingUInt("crosshairValue", defaultRet );;
	return getCrosshairValue();
}

unsigned int OmLocalPreferences::getCrosshairValue()
{
	return Instance()->mCrosshairValue;
}

void OmLocalPreferences::setCrosshairValue(unsigned int value)
{
	Instance()->mCrosshairValue = value;
	writeSettingUInt( "crosshairValue", value );
}

bool OmLocalPreferences::getDefault2DViewFrameIn3D()
{
	const bool defaultRet = false;
	Instance()->m2DViewFrameIn3D = readSettingBool("2DViewFrameIn3D", defaultRet );
	return get2DViewFrameIn3D();
}
bool OmLocalPreferences::get2DViewFrameIn3D()
{
	return Instance()->m2DViewFrameIn3D;
}

void OmLocalPreferences::set2DViewFrameIn3D(bool value)
{
	Instance()->m2DViewFrameIn3D = value;
	writeSettingBool("2DViewFrameIn3D", value);
}

bool OmLocalPreferences::getDefaultDrawCrosshairsIn3D()
{
	const bool defaultRet = false;
	Instance()->mDrawCrosshairsIn3D = readSettingBool( "DrawCrosshairsIn3D", defaultRet );
	return getDrawCrosshairsIn3D();
}

bool OmLocalPreferences::getDrawCrosshairsIn3D()
{
	return Instance()->mDrawCrosshairsIn3D;
}

void OmLocalPreferences::setDrawCrosshairsIn3D(bool value)
{
	Instance()->mDrawCrosshairsIn3D = value;
	writeSettingBool("DrawCrosshairsIn3D", value);
}

bool OmLocalPreferences::getDefaultDoDiscoBall()
{
        const bool defaultRet = true;
        Instance()->mDoDiscoBall = readSettingBool( "DoDiscoBall", defaultRet );
        return getDoDiscoBall();
}

bool OmLocalPreferences::getDoDiscoBall()
{
        return Instance()->mDoDiscoBall;
}

void OmLocalPreferences::setDoDiscoBall(bool value)
{
        Instance()->mDoDiscoBall = value;
        writeSettingBool("DoDiscoBall", value);
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

void OmLocalPreferences::setScratchPath( QString scratchPath )
{
	writeSettingQString( "sratchPath", scratchPath );
}

QString OmLocalPreferences::getScratchPath( )
{
	QString defaultPath = "/tmp";
	return readSettingQString( "sratchPath", defaultPath );
}

