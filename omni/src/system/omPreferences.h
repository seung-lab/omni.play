#ifndef OM_PREFERENCES_H
#define OM_PREFERENCES_H

/*
 *	Preferences model adopted from:
 *	http://www.misfitcode.com/misfitmodel3d/
 *
 *	Brett Warne - bwarne@mit.edu - 4/8/09
 */

#include "common/omCommon.h"
#include "system/omEvents.h"
#include "system/omPreferenceDefinitions.h"

#include <QHash>
#include "zi/omUtility.h"

class OmProject;

class OmPreferences : boost::noncopyable {
public:
	static void Delete(){
		Instance().stringPrefs.clear();
		Instance().floatPrefs.clear();
		Instance().intPrefs.clear();
		Instance().boolPrefs.clear();
		Instance().v3fPrefs.clear();
	}

	static std::string GetString(const OM::PrefEnum key){
		return Instance().stringPrefs.value(key).toStdString();
	}
	static void SetString(const OM::PrefEnum key, const std::string& value){
		Instance().stringPrefs[ key ] = QString::fromStdString(value);
		postEvent(key);
	}

	static float GetFloat(const OM::PrefEnum key){
		return Instance().floatPrefs.value(key);
	}
	static void SetFloat(const OM::PrefEnum key, const float value){
		Instance().floatPrefs[key] = value;
		postEvent(key);
	}

	static int GetInteger(const OM::PrefEnum key){
		return Instance().intPrefs.value(key);
	}
	static void SetInteger(const OM::PrefEnum key, const int value){
		Instance().intPrefs[key] = value;
		postEvent(key);
	}

	static bool GetBoolean(const OM::PrefEnum key){
		return Instance().boolPrefs.value(key);
	}
	static void SetBoolean(const OM::PrefEnum key, const bool value){
		Instance().boolPrefs[key] = value;
		postEvent(key);
	}

	static Vector3f GetVector3f(const OM::PrefEnum key){
		return Instance().v3fPrefs.value(key);
	}
	static void SetVector3f(const OM::PrefEnum key, const Vector3f& value){
		Instance().v3fPrefs[key] = value;
		postEvent(key);
	}

private:
	OmPreferences(){}
	~OmPreferences(){}
	static inline OmPreferences& Instance(){
		return zi::singleton<OmPreferences>::instance();
	}

	QHash< int, QString > stringPrefs;
	QHash< int, float > floatPrefs;
	QHash< int, int > intPrefs;
	QHash< int, bool > boolPrefs;
	QHash< int, Vector3f> v3fPrefs;

	static void postEvent(const OM::PrefEnum key){
		OmEvents::PreferenceChange(key);
	}

	friend class zi::singleton_::singleton<OmPreferences>;
	friend QDataStream &operator<<(QDataStream & out, const OmPreferences & v );
	friend QDataStream &operator>>(QDataStream & in, OmPreferences & v );
	friend QDataStream &operator<<(QDataStream & out, const OmProject & p);
	friend QDataStream &operator>>(QDataStream & in, OmProject & p);
};

#endif
