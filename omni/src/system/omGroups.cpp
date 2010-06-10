#include "system/omGroups.h"
#include "common/omDebug.h"
#include "system/omManageableObject.h"
#include "system/omGroup.h"

OmGroups::OmGroups(OmSegmentation * seg) 
	: mSegmentation(seg)
{
	debug("groups", "construting group for seg\n");
}

OmGroups::~OmGroups()
{
}

OmGroup & OmGroups::AddGroup(OmGroupName name)
{
	debug("groups", "adding group for seg\n");
	OmGroup & group = mGroupManager.Add();
	group.mName = name;

	mGroupsByName.insert(name, group.GetId());

	return group;
}

OmGroup & OmGroups::setGroupIDs(const OmSegIDsSet & set, OmGroup & group)
{
	return group;
}

OmGroup & OmGroups::SetGroup(const OmSegIDsSet & set, OmGroupName name)
{
	if(!mGroupsByName[name]) {
		return setGroupIDs(set, AddGroup(name));
	} else {
		return setGroupIDs(set, GetGroup(name));
	}
}

OmGroup & OmGroups::GetGroup(OmGroupName name)
{
	return mGroupManager.Get(mGroupsByName[name]);
}


