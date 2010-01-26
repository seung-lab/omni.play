
#include "omFilterManager.h"
#include "omFilter.h"

#include <vtkImageData.h>
#include "system/omDebug.h"

#define DEBUG 0



#pragma mark -
#pragma mark OmFilterManager
/////////////////////////////////
///////
///////		 OmFilterManager
///////
OmFilterManager::OmFilterManager() {
}


#pragma mark 
#pragma mark Filter Management
/////////////////////////////////
///////		Filter Management


OmFilter& 
OmFilterManager::AddFilter() {
	OmFilter &r_filter = mGenericFilterManager.Add();
	
	return r_filter;
}



OmFilter& OmFilterManager::GetFilter(OmId id)
{
	return mGenericFilterManager.Get(id);
}

const set<OmId>& OmFilterManager::GetValidFilterIds()
{
	return mGenericFilterManager.GetValidIds();
}

bool OmFilterManager::IsFilterEnabled(OmId id)
{
	return mGenericFilterManager.IsEnabled(id);
}





#pragma mark 
#pragma mark DataMap Methods
/////////////////////////////////
///////		 DataMap Methods


#pragma mark 
#pragma mark Cached Filter Data Values Methods
/////////////////////////////////
///////		 Cached Filter Data Values Methods






#pragma mark 
#pragma mark Accessor Methods
/////////////////////////////////
///////		 Accessor Methods

