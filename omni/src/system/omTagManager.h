#ifndef OM_TAG_MANAGER_H
#define OM_TAG_MANAGER_H

/*
 *	TagManager is a singleton wrapper for a GenericManager. 
 *
 *	Brett Warne - bwarne@mit.edu - 4/6/09
 */

#include "omManageableObject.h"
#include "omGenericManager.h"

#include "system/omSystemTypes.h"

#include "common/omStd.h"
#include "common/omSerialization.h"

//a tag is just a manageable object
typedef OmManageableObject OmTag;

class OmTagManager {

 public:

	static OmTagManager *Instance();
	static void Delete();

	//tag management
	 OmTag & GetTag(OmId id);
	 OmTag & AddTag(OmId id = NULL_OM_ID);
	void RemoveTag(OmId id);
	const OmIds & GetValidTagIds();

 protected:
	// singleton constructor, copy constructor, assignment operator protected
	 OmTagManager();
	~OmTagManager();
	 OmTagManager(const OmTagManager &);
	 OmTagManager & operator=(const OmTagManager &);

 private:
	//singleton
	static OmTagManager *mspInstance;

	//generic tag manager
	 OmGenericManager < OmTag > mManager;

	friend class boost::serialization::access;
	 template < class Archive > void serialize(Archive & ar, const unsigned int file_version);
};

#pragma mark
#pragma mark Serialization
/////////////////////////////////
///////          Serialization

BOOST_CLASS_VERSION(OmTagManager, 0)

template < class Archive > void
 OmTagManager::serialize(Archive & ar, const unsigned int file_version)
{
	ar & mManager;
}

#endif
