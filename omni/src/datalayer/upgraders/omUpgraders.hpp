#ifndef OM_UPGRADERS_HPP
#define OM_UPGRADERS_HPP

#include "datalayer/upgraders/omUpgradeTo14.hpp"
#include "datalayer/upgraders/omUpgradeTo15.hpp"

class OmUpgraders{
public:
	static void to14(){
		OmUpgradeTo14 u;
		u.copyDataOutFromHDF5();
	}

	static void to15(){
		OmUpgradeTo15 u;
		u.rewriteSegments();
	}
};

#endif
