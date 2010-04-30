
#include "omPreferences.h"
#include "omEventManager.h"
#include "events/omPreferenceEvent.h"
#include "common/omDebug.h"

#define DEBUG 0

//init instance pointer
OmPreferences *OmPreferences::mspInstance = 0;

/////////////////////////////////
///////
///////          OmStateManager
///////

OmPreferences::OmPreferences()
{

}

OmPreferences::~OmPreferences()
{

}

OmPreferences *OmPreferences::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmPreferences();
	}
	return mspInstance;
}

void OmPreferences::Delete()
{
	if (mspInstance){
		delete mspInstance;
	}
	mspInstance = NULL;
}

/////////////////////////////////
///////          Item Accessors

bool OmPreferences::ValidPreference(const int key)
{
	return Instance()->CheckKey(key);
}

bool OmPreferences::CheckKey(const int)
{
	return true;

	//TODO: check if key exists locallys; if not, set key from defaults in OmPreferenceDefinitions
}

/////////////////////////////////
///////          Accessors

string OmPreferences::GetString(const int key)
{
	CheckKey(key);
	return Instance()->stringPrefs.value(key).toStdString();
}

void OmPreferences::SetString(const int key, string value)
{
	Instance()->stringPrefs[ key ] = QString::fromStdString(value);
	OmEventManager::PostEvent(new OmPreferenceEvent(OmPreferenceEvent::PREFERENCE_CHANGE, key));
}

float OmPreferences::GetFloat(const int key)
{
	CheckKey(key);
	return Instance()->floatPrefs.value(key);
}

void OmPreferences::SetFloat(const int key, const float value)
{
	Instance()->floatPrefs[key] = value;
	OmEventManager::PostEvent(new OmPreferenceEvent(OmPreferenceEvent::PREFERENCE_CHANGE, key));
}

int OmPreferences::GetInteger(const int key)
{
	CheckKey(key);
	return Instance()->intPrefs.value(key);
}

void OmPreferences::SetInteger(const int key, const int value)
{
	Instance()->intPrefs[key] = value;
	OmEventManager::PostEvent(new OmPreferenceEvent(OmPreferenceEvent::PREFERENCE_CHANGE, key));
}

bool OmPreferences::GetBoolean(const int key)
{
	CheckKey(key);
	return Instance()->boolPrefs.value(key);
}

void OmPreferences::SetBoolean(const int key, const bool value)
{
	Instance()->boolPrefs[key] = value;
	OmEventManager::PostEvent(new OmPreferenceEvent(OmPreferenceEvent::PREFERENCE_CHANGE, key));
}

Vector3f OmPreferences::GetVector3f(const int key)
{
	CheckKey(key);
	return Instance()->v3fPrefs.value(key);
}

void OmPreferences::SetVector3f(const int key, Vector3f value)
{
	Instance()->v3fPrefs[key] = value;
	OmEventManager::PostEvent(new OmPreferenceEvent(OmPreferenceEvent::PREFERENCE_CHANGE, key));

}
