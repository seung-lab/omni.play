#ifndef OM_TAG_MANAGER_H
#define OM_TAG_MANAGER_H

/*
 *
 */

#include "common/omStd.h"
#include "common/omSerialization.h"

class OmTagManager {

 public:

	static OmTagManager *Instance();
	static void Delete();

 protected:
	// singleton constructor, copy constructor, assignment operator protected
	 OmTagManager();
	~OmTagManager();
	 OmTagManager(const OmTagManager &);
	 OmTagManager & operator=(const OmTagManager &);

 private:
	//singleton
	static OmTagManager *mspInstance;

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

}

#endif
