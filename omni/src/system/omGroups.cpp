#include "system/omGroups.h"
#include "common/omDebug.h"

OmGroups::OmGroups(OmSegmentation * seg) : mSegmentation(seg)
{
	debug("groups", "construting group for seg\n");
}

OmGroups::~OmGroups()
{
	mGroups.clear();
	foreach(OmGroup * group, mGroups) {
		delete group;
	}
}

OmId OmGroups::AddGroup(OmIds & ids)
{
	debug("groups", "adding group for seg\n");
	OmGroup * group = new OmGroup(ids);
	mGroups.append(group);
	return mGroups.size() - 1;
}


