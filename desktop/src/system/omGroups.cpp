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

OmGroup & OmGroups::AddGroup(om::common::GroupName name)
{
    //debug(groups, "adding group for seg\n");
    OmGroup & group = mGroupManager.Add();
    group.mName = name;

    mGroupsByName[name] = group.GetID();

    GroupsTable::Repopulate();

    return group;
}

void OmGroups::setGroupIDs(const om::common::SegIDSet & set, OmGroup * group, bool add)
{
    printf("adding ids\n");
    if(add) {
        group->AddIds(set);
    } else {
        group->RemoveIds(set);
    }
    GroupsTable::Repopulate(group->GetID());
}

void OmGroups::SetGroup(const om::common::SegIDSet & set, om::common::GroupName name)
{
    if(!mGroupsByName[name]) {
        setGroupIDs(set, &AddGroup(name), true);
    } else {
        setGroupIDs(set, &GetGroup(name), true);
    }
    GroupsTable::Repopulate();
}

void OmGroups::UnsetGroup(const om::common::SegIDSet & set, om::common::GroupName name)
{
    if(!mGroupsByName[name]) {
        return;
    } else {
        setGroupIDs(set, &GetGroup(name), false);
    }

    GroupsTable::Repopulate();
}

OmGroup & OmGroups::GetGroup(om::common::GroupName name)
{
    return mGroupManager.Get(mGroupsByName[name]);
}

OmGroup & OmGroups::GetGroup(om::common::GroupID id)
{
    return mGroupManager.Get(id);
}

om::common::GroupIDSet OmGroups::GetGroups()
{
    om::common::GroupIDSet set;
    FOR_EACH(iter, mGroupsByName)
    {
        set.insert(iter->second);
        printf("got\n");
    }
    return set;
}

om::common::GroupIDSet OmGroups::GetGroups(om::common::SegID seg)
{
    om::common::GroupIDSet set;

    FOR_EACH(iter, mGroupsByName)
    {
        OmGroup& group = GetGroup(iter->second);
        om::common::SegIDSet ids = group.GetIDs();
        if(ids.count(seg) > 0) {
            set.insert(iter->second);
        }
    }
    return set;
}

