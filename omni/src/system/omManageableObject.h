#ifndef OM_MANAGEABLE_OBJECT_H
#define OM_MANAGEABLE_OBJECT_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/3/09
 */


#include "common/omStd.h"
#include "system/omException.h"
#include "common/omSerialization.h"
#include "omSystemTypes.h"


class OmManageableObject {
public:
	OmManageableObject() { }
	
	OmManageableObject(OmId id)
	: mId(id) { }
	
	
	//accessors
	OmId GetId() { return mId; }
	
	const string& GetName() const { return mName; }
	void SetName(const string &name) { mName = name; }

	const string& GetNote() const { return mNote; }
	void SetNote(const string &note) { mNote = note; }
	
	const bool IsLocked() const { return mIsLocked; }
	void SetLocked(bool locked = true) { mIsLocked = locked; }
	
	virtual void Print() { };

protected:
	OmId mId;
	string mName;
	string mNote;
	int mIsLocked;
	
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};



#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmManageableObject, 0)

template<class Archive>
void 
OmManageableObject::serialize(Archive & ar, const unsigned int file_version) {
	
	ar & mId;
	ar & mName;
	ar & mNote;
	ar & mIsLocked;
}



#endif
