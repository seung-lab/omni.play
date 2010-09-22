#ifndef OM_UPGRADERS_HPP
#define OM_UPGRADERS_HPP

#include "datalayer/upgraders/omUpgradeTo14.hpp"

class OmUpgraders{
public:
	static void to14(){
		OmUpgradeTo14 u;
		u.copyDataOutFromHDF5();
	}

};

#endif
