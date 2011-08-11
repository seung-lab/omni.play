#pragma once

#include "common/omCommon.h"
#include "system/omManageableObject.h"
#include "utility/omColorUtils.hpp"
#include "datalayer/archive/segmentation.h"

class OmGroup : public OmManageableObject {
public:
    OmGroup(){}

    OmGroup(OmID id)
        : OmManageableObject(id)
    {
        mColor = OmColorUtils::GetRandomColor();
    }

    OmGroup(const OmSegIDsSet& ids)
    {
        mColor = OmColorUtils::GetRandomColor();
        AddIds(ids);
    }

    virtual ~OmGroup()
    {}

    void AddIds(const OmSegIDsSet& ids)
    {
        FOR_EACH(iter, ids) {
            mIDs.insert(*iter);
        }
    }

    void RemoveIds(const OmSegIDsSet& ids)
    {
        FOR_EACH(iter, ids) {
            mIDs.erase(*iter);
        }
    }

    OmGroupName GetName() const {
        return mName;
    }

    const OmSegIDsSet& GetIDs() const {
        return mIDs;
    }

private:
    OmSegIDsSet mIDs;
    OmColor mColor;

    OmGroupName mName;
    friend class OmGroups;

    friend YAML::Emitter &om::data::archive::operator<<(YAML::Emitter & out, const OmGroup & g );
    friend void om::data::archive::operator>>(const YAML::Node & in, OmGroup & g );
    friend QDataStream &operator<<(QDataStream & out, const OmGroup & g );
    friend QDataStream &operator>>(QDataStream & in, OmGroup & g );
};

