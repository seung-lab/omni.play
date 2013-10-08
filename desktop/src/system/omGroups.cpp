#include "system/omGroup.h"
#include "system/omGroups.h"
#include "system/omManageableObject.h"
#include "gui/groupsTable/groupsTable.h"

OmGroups::OmGroups(OmSegmentation * seg)
    : mSegmentation(seg)
{}

OmGroups::~OmGroups()
{}

OmGroup & OmGroups::AddGroup(std::string name)
{
    //debug(groups, "adding group for seg\n");
    OmGroup & group = mGroupManager.Add();
    group.mName = name;

    mGroupsByName.insert(QString::fromStdString(name), group.GetID());

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

void OmGroups::SetGroup(const om::common::SegIDSet & set, std::string s)
{
    QString name = QString::fromStdString(s);
    if(!mGroupsByName[name]) {
        setGroupIDs(set, &AddGroup(s), true);
    } else {
        setGroupIDs(set, &GetGroup(s), true);
    }
    GroupsTable::Repopulate();
}

void OmGroups::UnsetGroup(const om::common::SegIDSet & set, std::string s)
{
    QString name = QString::fromStdString(s);
    if(!mGroupsByName[name]) {
        return;
    } else {
        setGroupIDs(set, &GetGroup(s), false);
    }

    GroupsTable::Repopulate();
}

OmGroup & OmGroups::GetGroup(std::string s)
{
    QString name = QString::fromStdString(s);
    return mGroupManager.Get(mGroupsByName[name]);
}

OmGroup & OmGroups::GetGroup(uint32_t id)
{
    return mGroupManager.Get(id);
}

OmGroups::GroupIDSet OmGroups::GetGroups()
{
    OmGroups::GroupIDSet set;
    FOR_EACH(iter, mGroupsByName)
    {
        set.insert(*iter);
        printf("got\n");
    }
    return set;
}

OmGroups::GroupIDSet OmGroups::GetGroups(om::common::SegID seg)
{
    OmGroups::GroupIDSet set;

    FOR_EACH(iter, mGroupsByName)
    {
        auto& group = GetGroup(*iter);
        auto ids = group.GetIDs();
        if(ids.count(seg) > 0) {
            set.insert(*iter);
        }
    }
    return set;
}

