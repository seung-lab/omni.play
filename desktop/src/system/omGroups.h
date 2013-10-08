#pragma once

#include "common/common.h"
#include "system/omGenericManager.hpp"
#include "datalayer/archive/segmentation.h"
#include <QVector>
#include <QHash>
#include <string>
#include <unordered_set>

class OmSegmentation;
class OmGroup;

class OmGroups : boost::noncopyable {
public:
    typedef std::unordered_set<uint32_t> GroupIDSet;

    OmGroups(OmSegmentation * seg);
    ~OmGroups();

    OmGroup & AddGroup(std::string);
    OmGroup & GetGroup(uint32_t);
    OmGroup & GetGroup(std::string);

    uint32_t GetIDFromName(std::string);
    void SetGroup(const om::common::SegIDSet & set, std::string name);
    void UnsetGroup(const om::common::SegIDSet & set, std::string name);

    GroupIDSet GetGroups();
    GroupIDSet GetGroups(om::common::SegID);

    void populateGroupsList();


private:
    void setGroupIDs(const om::common::SegIDSet & set, OmGroup * group, bool doSet);

    OmSegmentation * mSegmentation;
    OmGenericManager<OmGroup> mGroupManager;
    QHash<QString, uint32_t> mGroupsByName;

    friend YAMLold::Emitter &YAMLold::operator<<(YAMLold::Emitter & out, const OmGroups &);
    friend void YAMLold::operator>>(const YAMLold::Node & in, OmGroups &);
    friend QDataStream &operator<<(QDataStream & out, const OmGroups &);
    friend QDataStream &operator>>(QDataStream & in, OmGroups &);
};

