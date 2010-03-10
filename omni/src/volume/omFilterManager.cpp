
#include "omFilterManager.h"
#include "omFilter.h"

#include <vtkImageData.h>
#include "system/omDebug.h"

#define DEBUG 0

/////////////////////////////////
///////
///////          OmFilterManager
///////
OmFilterManager::OmFilterManager()
{
}

/////////////////////////////////
///////         Filter Management

OmFilter & OmFilterManager::AddFilter()
{
	OmFilter & r_filter = mGenericFilterManager.Add();

	return r_filter;
}

OmFilter & OmFilterManager::GetFilter(OmId id)
{
	return mGenericFilterManager.Get(id);
}

const set < OmId > &OmFilterManager::GetValidFilterIds()
{
	return mGenericFilterManager.GetValidIds();
}

bool OmFilterManager::IsFilterEnabled(OmId id)
{
	return mGenericFilterManager.IsEnabled(id);
}

/////////////////////////////////
///////          DataMap Methods

/////////////////////////////////
///////          Cached Filter Data Values Methods

/////////////////////////////////
///////          Accessor Methods
