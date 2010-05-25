#include "system/omGroups.h"
#include "common/omDebug.h"

OmGroups::OmGroups(OmSegmentation * seg) : mSegmentation(seg)
{
	debug("groups", "construting group for seg\n");
}

OmGroups::~OmGroups()
{
}

OmId OmGroups::AddGroup(OmIds & ids)
{
	debug("groups", "adding group for seg\n");
	mGroups.append(ids);
	return mGroups.size() - 1;
}


