#ifndef OM_LOCAL_CONFIGURATION_H
#define OM_LOCAL_CONFIGURATION_H

#include <QString>
#include "utility/localPrefFiles.h"

class OmLocalPreferences {

public:
	static OmLocalPreferences* Instance();
	static void Delete();

	static int get_num_cores();

	static int numAllowedWorkerThreads();
	static void setNumAllowedWorkerThreads(int);

	static bool getStoreMeshesInTempFolder();
	static void setStoreMeshesInTempFolder( const bool );

	static unsigned int getRamCacheSizeMB();
	static void setRamCacheSizeMB(const unsigned int);

	static unsigned int getVRamCacheSizeMB();
	static void setVRamCacheSizeMB(const unsigned int);

	static bool getStickyCrosshairMode();
	static void setStickyCrosshairMode(bool sticky);

	static unsigned int getCrosshairValue();
	static unsigned int getDefaultCrosshairValue();
	static void setCrosshairValue(const unsigned int);

	static bool get2DViewFrameIn3D();
	static bool getDefault2DViewFrameIn3D();
	static void set2DViewFrameIn3D(const bool);

	static bool getDrawCrosshairsIn3D();
	static bool getDefaultDrawCrosshairsIn3D();
	static void setDrawCrosshairsIn3D(const bool);

        static bool getDoDiscoBall();
        static bool getDefaultDoDiscoBall();
        static void setDoDiscoBall(const bool);

	static QStringList getRecentlyUsedFilesNames();
	static void setRecentlyUsedFilesNames( QStringList values);

	static QString getScratchPath( );
	static void setScratchPath( QString value);


protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmLocalPreferences();
	~OmLocalPreferences();
	OmLocalPreferences(const OmLocalPreferences&);
	OmLocalPreferences& operator= (const OmLocalPreferences&);

private:
	//singleton
	static OmLocalPreferences* mspInstance;

	static bool readSettingBool( QString setting, const bool defaultRet );
	static void writeSettingBool( QString setting, const bool value );

	static int readSettingInt( QString setting, const int defaultRet );
	static void writeSettingInt( QString setting, const int value );

	static unsigned int readSettingUInt( QString setting, const unsigned int defaultRet );
	static void writeSettingUInt( QString setting, const unsigned int value );

	static QStringList readSettingQStringList( QString setting, QStringList defaultRet );

	static void writeSettingQString (QString setting, QString value);
	static QString readSettingQString (QString setting, QString defaultRet);

	static bool settingExists( QString setting );

	// Global Variables for Quick access to Preference Settings
	// Thus bypassing any file I/O
	bool * stickyCrosshairMode;
	bool m2DViewFrameIn3D;
	bool mDrawCrosshairsIn3D;
	bool mDoDiscoBall;
	int mCrosshairValue;
	LocalPrefFiles* localPrefFiles;

};

#endif
