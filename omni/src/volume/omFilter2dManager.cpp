
#include "omFilter2dManager.h"
#include "omFilter2d.h"

#include <vtkImageData.h>

#define DEBUG 0



/////////////////////////////////
///////
///////		 OmFilterManager
///////
OmFilter2dManager::OmFilter2dManager() {
}


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
bool OmFilter2dManager::IsFilterValid(OmId id)
{
	return mGenericFilterManager.IsValid(id);
}




/////////////////////////////////
///////		 DataMap Methods


/////////////////////////////////
///////		 Cached Filter Data Values Methods






/////////////////////////////////
///////		 Accessor Methods

