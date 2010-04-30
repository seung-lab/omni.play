#ifndef OM_PREFERENCES_H
#define OM_PREFERENCES_H

/*
 *	Preferences model adopted from:
 *	http://www.misfitcode.com/misfitmodel3d/
 *
 *	Brett Warne - bwarne@mit.edu - 4/8/09
 */

#include <QHash>
#include "common/omCommon.h"

class OmPreferences {

public:
	
	static OmPreferences* Instance();
	static void Delete();
	
	static bool ValidPreference(const int key);
	
	//accessors
	static string GetString(const int);
	static void SetString(const int, string );

	static float GetFloat(const int);
	static void SetFloat(const int, float);

	static int GetInteger(const int);
	static void SetInteger(const int, int);

	static bool GetBoolean(const int);
	static void SetBoolean(const int, bool);

	static Vector3f GetVector3f(const int);
	static void SetVector3f(const int, Vector3f);
	
protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmPreferences();
	~OmPreferences();
	OmPreferences(const OmPreferences&);
	OmPreferences& operator= (const OmPreferences&);
	
private:
	static bool CheckKey(const int key );
	
	//singleton
	static OmPreferences* mspInstance;
	
	//preference map
	QHash< int, QString > stringPrefs;
	QHash< int, float > floatPrefs;
	QHash< int, int > intPrefs;
	QHash< int, bool > boolPrefs;
	QHash< int, Vector3f> v3fPrefs;

	friend QDataStream &operator<<(QDataStream & out, const OmPreferences & v );
	friend QDataStream &operator>>(QDataStream & in, OmPreferences & v );
};

#endif
