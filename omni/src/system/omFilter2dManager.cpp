
#include "omFilter2dManager.h"
#include "omFilter2d.h"

#include <vtkImageData.h>

#define DEBUG 0



#pragma mark -
#pragma mark OmFilterManager
/////////////////////////////////
///////
///////		 OmFilterManager
///////
OmFilter2dManager::OmFilter2dManager() {
}


#pragma mark 
#pragma mark Filter Management
/////////////////////////////////
///////		Filter Management


OmFilter2d& 
OmFilter2dManager::AddFilter() {
	OmFilter2d &r_filter = mGenericFilterManager.Add();
	
	return r_filter;
}



OmFilter2d& OmFilter2dManager::GetFilter(OmId id)
{
	return mGenericFilterManager.Get(id);
}

const set<OmId>& OmFilter2dManager::GetValidFilterIds()
{
	return mGenericFilterManager.GetValidIds();
}

bool OmFilter2dManager::IsFilterEnabled(OmId id)
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

