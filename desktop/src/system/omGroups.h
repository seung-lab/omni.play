#pragma once

#include "common/common.h"
#include "common/genericManager.hpp"
#include "datalayer/archive/segmentation.h"
#include <boost/unordered_map.hpp>
#include <QVector>

class OmSegmentation;
class OmGroup;

class OmGroups : boost::noncopyable {
public:
    OmGroups(OmSegmentation * seg);
    ~OmGroups();

    OmGroup & AddGroup(om::common::GroupName);
    OmGroup & GetGroup(om::common::GroupID);
    OmGroup & GetGroup(om::common::GroupName);

    om::common::GroupID GetIDFromName(om::common::GroupName);
    void SetGroup(const om::common::SegIDSet & set, om::common::GroupName name);
    void UnsetGroup(const om::common::SegIDSet & set, om::common::GroupName name);

    om::common::GroupIDSet GetGroups();
    om::common::GroupIDSet GetGroups(om::common::SegID);

    om::common::ID GetSegmentationID();
    void populateGroupsList();


private:
    void setGroupIDs(const om::common::SegIDSet & set, OmGroup * group, bool doSet);

    OmSegmentation * mSegmentation;
    om::common::GenericManager<OmGroup> mGroupManager;
    boost::unordered_map<om::common::GroupName, om::common::GroupID> mGroupsByName;

    friend YAML::Emitter &YAML::operator<<(YAML::Emitter & out, const OmGroups &);
    friend void YAML::operator>>(const YAML::Node & in, OmGroups &);
    friend QDataStream &operator<<(QDataStream & out, const OmGroups &);
    friend QDataStream &operator>>(QDataStream & in, OmGroups &);
};

