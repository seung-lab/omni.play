#ifndef OM_FILTER_MANAGER_H
#define OM_FILTER_MANAGER_H

/*
 *	Matt Wimer mwimer@mit.edu Oct13, 2009
 */

#include "common/omCommon.h"
#include "volume/omFilter2d.h"
#include "system/omGenericManager.h"

class OmFilter2dManager {

public:
	OmFilter2dManager();
	
	OmFilter2d& AddFilter();
	OmFilter2d& GetFilter(OmId id);
	const OmIds & GetValidFilterIds();
	bool IsFilterEnabled(OmId id);
	bool IsFilterValid(OmId id);

	
	//properties?
	//data map?
	//data values?
	
private:
	//filter management
	OmGenericManager< OmFilter2d > mGenericFilterManager;
	
	friend QDataStream &operator<<(QDataStream & out, const OmFilter2dManager & );
	friend QDataStream &operator>>(QDataStream & in, OmFilter2dManager & );
};

#endif
