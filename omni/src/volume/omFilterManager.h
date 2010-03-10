#ifndef OM_FILTER_MANAGER_H
#define OM_FILTER_MANAGER_H

/*
 *	Matt Wimer mwimer@mit.edu Oct13, 2009
 */




#include "system/omFilter.h"
#include "system/omGenericManager.h"
#include "common/omStd.h"



class OmFilterManager {

public:
	OmFilterManager();
	
	
	OmFilter& AddFilter();
	OmFilter& GetFilter(OmId id);
	const set<OmId>& GetValidFilterIds();
	bool IsFilterEnabled(OmId id);


	
	//properties?
	//data map?
	//data values?
	
private:
	//filter management
	OmGenericManager< OmFilter > mGenericFilterManager;
	
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};






/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmFilterManager, 0)

template<class Archive>
void 
OmFilterManager::serialize(Archive & ar, const unsigned int file_version) {
	ar & mGenericFilterManager;
}



#endif
