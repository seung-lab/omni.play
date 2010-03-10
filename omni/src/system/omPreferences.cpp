
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
		mspInstance = new OmPreferences;
	}
	return mspInstance;
}

void OmPreferences::Delete()
{
	if (mspInstance)
		delete mspInstance;
	mspInstance = NULL;
}

/////////////////////////////////
///////          Item Accessors

bool OmPreferences::ValidPreference(const int key)
{
	return Instance()->mPreferenceMap.count(key);
}

const OmPrefItem & OmPreferences::GetItem(const int key, const OmPrefItem::OmPrefItemType & type)
{
	//check key exists
	if (!ValidPreference(key)) {
		//debug("FIXME", << "OmPreferences::GetItem: Preference key: " << key << " does not exist." << endl;
		assert(false);
	}
	//get item
	OmPrefItem & item = Instance()->mPreferenceMap[key];

	//check item type
	if (item.mType != type) {
		//debug("FIXME", << "OmPreferences::GetItem: Preference key: " << key << " has unexpected type." << endl;
		//debug("FIXME", << "OmPreferences::GetItem: Expected key type: " << type << " but found: " << item.mType << endl;
		assert(false);
	}

	return item;
}

void OmPreferences::SetItem(const int key, const OmPrefItem & item)
{
	//set preference
	Instance()->mPreferenceMap[key] = item;
	//post preference change event
	OmEventManager::PostEvent(new OmPreferenceEvent(OmPreferenceEvent::PREFERENCE_CHANGE, key));
}

void OmPreferences::RefreshPreference(const int key)
{
	//check key exists
	if (!ValidPreference(key)) {
		//debug("FIXME", << "OmPreferences::GetItem: Preference key: " << key << " does not exist." << endl;
		assert(false);
	}
	//get item
	OmPrefItem & item = Instance()->mPreferenceMap[key];

	//set item
	SetItem(key, item);
}

/////////////////////////////////
///////          Accessors

const string & OmPreferences::GetString(const int key)
{
	return GetItem(key, OmPrefItem::STRING_TYPE).mString;
}

void OmPreferences::SetString(const int key, const string & value)
{
	OmPrefItem item;
	item.mType = OmPrefItem::STRING_TYPE;
	item.mString = value;

	SetItem(key, item);
}

void OmPreferences::Get(const int key, string & value)
{
	value = GetString(key);
}

void OmPreferences::Set(const int key, const string & value)
{
	SetString(key, value);
}

void OmPreferences::Set(const int key, const char *c_str)
{
	SetString(key, c_str);
}

float OmPreferences::GetFloat(const int key)
{
	return GetItem(key, OmPrefItem::FLOAT_TYPE).mFloat;
}

void OmPreferences::SetFloat(const int key, const float value)
{
	OmPrefItem item;
	item.mType = OmPrefItem::FLOAT_TYPE;
	item.mFloat = value;

	SetItem(key, item);
}

void OmPreferences::Get(const int key, float &value)
{
	value = GetFloat(key);
}

void OmPreferences::Set(const int key, const float value)
{
	SetFloat(key, value);
}

int OmPreferences::GetInteger(const int key)
{
	return GetItem(key, OmPrefItem::INTEGER_TYPE).mInteger;
}

void OmPreferences::SetInteger(const int key, const int value)
{
	OmPrefItem item;
	item.mType = OmPrefItem::INTEGER_TYPE;
	item.mInteger = value;

	SetItem(key, item);
}

void OmPreferences::Get(const int key, int &value)
{
	value = GetInteger(key);
}

void OmPreferences::Set(const int key, const int value)
{
	SetInteger(key, value);
}

bool OmPreferences::GetBoolean(const int key)
{
	return GetItem(key, OmPrefItem::BOOLEAN_TYPE).mBoolean;
}

void OmPreferences::SetBoolean(const int key, const bool value)
{
	OmPrefItem item;
	item.mType = OmPrefItem::BOOLEAN_TYPE;
	item.mBoolean = value;

	SetItem(key, item);
}

void OmPreferences::Get(const int key, bool & value)
{
	value = GetBoolean(key);
}

void OmPreferences::Set(const int key, const bool value)
{
	SetBoolean(key, value);
}

const Vector3f & OmPreferences::GetVector3f(const int key)
{
	return GetItem(key, OmPrefItem::VECTOR3F_TYPE).mVector3f;
}

void OmPreferences::SetVector3f(const int key, const Vector3f & value)
{
	OmPrefItem item;
	item.mType = OmPrefItem::VECTOR3F_TYPE;
	item.mVector3f = value;

	SetItem(key, item);
}

void OmPreferences::Get(const int key, Vector3f & value)
{
	value = GetVector3f(key);
}

void OmPreferences::Set(const int key, const Vector3f & value)
{
	SetVector3f(key, value);
}
