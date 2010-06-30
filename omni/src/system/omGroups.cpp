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

void OmGroups::setGroupIDs(const OmSegIDsSet & set, OmGroup * group, bool doSet)
{
}

void OmGroups::SetGroup(const OmSegIDsSet & set, OmGroupName name)
{
	if(!mGroupsByName[name]) {
		setGroupIDs(set, &AddGroup(name), true);
	} else {
		setGroupIDs(set, &GetGroup(name), true);
	}
}

void OmGroups::UnsetGroup(const OmSegIDsSet & set, OmGroupName name)
{
        if(!mGroupsByName[name]) {
                return;
        } else {
                setGroupIDs(set, &GetGroup(name), false);
        }
}

OmGroup & OmGroups::GetGroup(OmGroupName name)
{
	return mGroupManager.Get(mGroupsByName[name]);
}


