#include "actions/omActions.h"
#include "omFilter2dManager.h"
#include "omFilter2d.h"

OmFilter2dManager::OmFilter2dManager()
{}

OmFilter2d& OmFilter2dManager::AddFilter()
{
    OmFilter2d &r_filter = filters_.Add();
    OmActions::Save();
    return r_filter;
}
