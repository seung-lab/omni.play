#include "omFilter2dManager.h"
#include "omFilter2d.h"

OmFilter2dManager::OmFilter2dManager()
{
}

OmFilter2d& OmFilter2dManager::AddFilter()
{
	OmFilter2d &r_filter = mGenericFilterManager.Add();

	return r_filter;
}

OmFilter2d& OmFilter2dManager::GetFilter(OmID id)
{
	return mGenericFilterManager.Get(id);
}

const OmIDsSet & OmFilter2dManager::GetValidFilterIds()
{
	return mGenericFilterManager.GetValidIds();
}

bool OmFilter2dManager::IsFilterEnabled(OmID id)
{
	return mGenericFilterManager.IsEnabled(id);
}

bool OmFilter2dManager::IsFilterValid(OmID id)
{
	return mGenericFilterManager.IsValid(id);
}
