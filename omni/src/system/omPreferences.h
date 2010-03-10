#ifndef OM_PREFERENCES_H
#define OM_PREFERENCES_H

/*
 *	Preferences model adopted from:
 *	http://www.misfitcode.com/misfitmodel3d/
 *
 *	Brett Warne - bwarne@mit.edu - 4/8/09
 */


#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;

#include "common/omSerialization.h"
#include "common/omStd.h"


class OmPrefItem {
	
	enum OmPrefItemType {
		BOOLEAN_TYPE,
		INTEGER_TYPE,
		FLOAT_TYPE,
		STRING_TYPE,
		VECTOR3F_TYPE
	};
	
	OmPrefItemType mType;

	string mString;
	Vector3f mVector3f;
	
	//union primitives to save space
	union {
		float mFloat;
		int mInteger;
		bool mBoolean;
	};
	
	
	friend class OmPreferences;
	friend class boost::serialization::access;
	
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};






class OmPreferences {

public:
	
	static OmPreferences* Instance();
	static void Delete();
	

	static bool ValidPreference(const int key);
	static void RefreshPreference(const int key);
	
	//accessors
	static const string& GetString(const int);
	static void SetString(const int, const string&);
	static void Get(const int, string&);
	static void Set(const int, const string&);
	static void Set(const int, const char *);
	
	
	static float GetFloat(const int);
	static void SetFloat(const int, float);
	static void Get(const int, float&);
	static void Set(const int, float);
	
	
	static int GetInteger(const int);
	static void SetInteger(const int, int);
	static void Get(const int, int&);
	static void Set(const int, int);
	
	
	static bool GetBoolean(const int);
	static void SetBoolean(const int, bool);
	static void Get(const int, bool&);
	static void Set(const int, bool);
	

	static const Vector3f& GetVector3f(const int);
	static void SetVector3f(const int, const Vector3f&);
	static void Get(const int, Vector3f&);
	static void Set(const int, const Vector3f&);
	
protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmPreferences();
	~OmPreferences();
	OmPreferences(const OmPreferences&);
	OmPreferences& operator= (const OmPreferences&);

	
private:
	static const OmPrefItem& GetItem(const int key, const OmPrefItem::OmPrefItemType &type);
	static void SetItem(const int, const OmPrefItem&);
	
	//singleton
	static OmPreferences* mspInstance;
	
	//preference map
	map< int, OmPrefItem > mPreferenceMap;
	
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};







/////////////////////////////////
///////		 Serialization

BOOST_CLASS_VERSION(OmPrefItem, 0)

template<class Archive>
void 
OmPrefItem::serialize(Archive & ar, const unsigned int file_version) {
	
	//load type
	ar & mType;

	//switch on type
	switch(mType) {
		case STRING_TYPE:
			ar & mString;
			break;
		case FLOAT_TYPE:
			ar & mFloat;
			break;
		case INTEGER_TYPE:
			ar & mInteger;
			break;
		case BOOLEAN_TYPE:
			ar & mBoolean;
			break;
		case VECTOR3F_TYPE:
			ar & mVector3f;
			break;
		default:
			assert(false);
	}
}




BOOST_CLASS_VERSION(OmPreferences, 0)

template<class Archive>
void 
OmPreferences::serialize(Archive & ar, const unsigned int file_version) {
	ar & mPreferenceMap;
}



#endif
