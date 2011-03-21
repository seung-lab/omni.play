#include "common/omDebug.h"
#include "system/omGroup.h"
#include "system/omGroups.h"
#include "system/omManageableObject.h"
#include "gui/groupsTable/groupsTable.h"

OmGroups::OmGroups(OmSegmentation * seg)
    : mSegmentation(seg)
{}

OmGroups::~OmGroups()
{}

OmGroup & OmGroups::AddGroup(OmGroupName name)
{
    //debug(groups, "adding group for seg\n");
    OmGroup & group = mGroupManager.Add();
    group.mName = name;

    mGroupsByName.insert(name, group.GetID());

    GroupsTable::Repopulate();

    return group;
}

void OmGroups::setGroupIDs(const OmSegIDsSet & set, OmGroup * group, bool add)
{
    printf("adding ids\n");
    if(add) {
        group->AddIds(set);
    } else {
        group->RemoveIds(set);
    }
    GroupsTable::Repopulate(group->GetID());
}

void OmGroups::SetGroup(const OmSegIDsSet & set, OmGroupName name)
{
    if(!mGroupsByName[name]) {
        setGroupIDs(set, &AddGroup(name), true);
    } else {
        setGroupIDs(set, &GetGroup(name), true);
    }
    GroupsTable::Repopulate();
}

void OmGroups::UnsetGroup(const OmSegIDsSet & set, OmGroupName name)
{
    if(!mGroupsByName[name]) {
        return;
    } else {
        setGroupIDs(set, &GetGroup(name), false);
    }

    GroupsTable::Repopulate();
}

OmGroup & OmGroups::GetGroup(OmGroupName name)
{
    return mGroupManager.Get(mGroupsByName[name]);
}

OmGroup & OmGroups::GetGroup(OmGroupID id)
{
    return mGroupManager.Get(id);
}

OmGroupIDsSet OmGroups::GetGroups()
{
    OmGroupIDsSet set;
    FOR_EACH(iter, mGroupsByName) {
        set.insert(*iter);
        printf("got\n");
    }
    return set;
}

OmGroupIDsSet OmGroups::GetGroups(OmSegID seg)
{
    OmGroupIDsSet set;
    FOR_EACH(iter, mGroupsByName) {
        OmGroup& group = GetGroup(*iter);
        OmSegIDsSet ids = group.GetIDs();
        if(ids.count(seg) > 0) {
            set.insert(*iter);
        }
    }
    return set;
}

