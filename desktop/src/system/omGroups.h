#pragma once

#include "common/common.h"
#include "system/omGenericManager.hpp"
#include "datalayer/archive/segmentation.h"
#include <QVector>
#include <QHash>

class OmSegmentation;
class OmGroup;

class OmGroups : boost::noncopyable {
public:
    OmGroups(OmSegmentation * seg);
    ~OmGroups();

    OmGroup & AddGroup(OmGroupName);
    OmGroup & GetGroup(OmGroupID);
    OmGroup & GetGroup(OmGroupName);

    OmGroupID GetIDFromName(OmGroupName);
    void SetGroup(const OmSegIDsSet & set, OmGroupName name);
    void UnsetGroup(const OmSegIDsSet & set, OmGroupName name);

    OmGroupIDsSet GetGroups();
    OmGroupIDsSet GetGroups(OmSegID);

    OmID GetSegmentationID();
    void populateGroupsList();


private:
    void setGroupIDs(const OmSegIDsSet & set, OmGroup * group, bool doSet);

    OmSegmentation * mSegmentation;
    OmGenericManager<OmGroup> mGroupManager;
    QHash<OmGroupName, OmGroupID> mGroupsByName;

    friend YAML::Emitter &YAML::operator<<(YAML::Emitter & out, const OmGroups &);
    friend void YAML::operator>>(const YAML::Node & in, OmGroups &);
    friend QDataStream &operator<<(QDataStream & out, const OmGroups &);
    friend QDataStream &operator>>(QDataStream & in, OmGroups &);
};

