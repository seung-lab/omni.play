#ifndef OM_FILTER_MANAGER_H
#define OM_FILTER_MANAGER_H

/*
 *	Matt Wimer mwimer@mit.edu Oct13, 2009
 */




#include "system/omFilter2d.h"
#include "system/omGenericManager.h"
#include "common/omStd.h"



class OmFilter2dManager {

public:
	OmFilter2dManager();
	
	
	OmFilter2d& AddFilter();
	OmFilter2d& GetFilter(OmId id);
	const set<OmId>& GetValidFilterIds();
	bool IsFilterEnabled(OmId id);


	
	//properties?
	//data map?
	//data values?
	
private:
	//filter management
	OmGenericManager< OmFilter2d > mGenericFilterManager;
	
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};






#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmFilter2dManager, 0)

template<class Archive>
void 
OmFilter2dManager::serialize(Archive & ar, const unsigned int file_version) {
	ar & mGenericFilterManager;
}



#endif
