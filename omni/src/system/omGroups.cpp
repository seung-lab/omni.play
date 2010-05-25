#include "system/omGroups.h"

OmGroups::OmGroups(OmSegmentation * seg) : mSegmentation(seg)
{
}

OmGroups::~OmGroups()
{
}

OmId OmGroups::AddGroup(OmIds & ids)
{
	mGroups.append(ids);
	return mGroups.size() - 1;
}


