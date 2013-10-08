#pragma once

#include "common/common.h"
#include "common/colors.h"
#include "system/omManageableObject.h"
#include "utility/color.hpp"
#include "datalayer/archive/segmentation.h"

class OmGroup : public OmManageableObject {
public:

    OmGroup(){}

    OmGroup(om::common::ID id)
        : OmManageableObject(id)
    {
        mColor =  om::utils::color::GetRandomColor();
    }

    OmGroup(const om::common::SegIDSet& ids)
    {
        mColor =  om::utils::color::GetRandomColor();
        AddIds(ids);
    }

    virtual ~OmGroup()
    {}

    void AddIds(const om::common::SegIDSet& ids)
    {
        FOR_EACH(iter, ids) {
            mIDs.insert(*iter);
        }
    }

    void RemoveIds(const om::common::SegIDSet& ids)
    {
        FOR_EACH(iter, ids) {
            mIDs.erase(*iter);
        }
    }

    std::string GetName() const {
        return mName;
    }

    const om::common::SegIDSet& GetIDs() const {
        return mIDs;
    }

private:
    om::common::SegIDSet mIDs;
    om::common::Color mColor;

    std::string mName;
    friend class OmGroups;

    friend YAMLold::Emitter &YAMLold::operator<<(YAMLold::Emitter & out, const OmGroup & g );
    friend void YAMLold::operator>>(const YAMLold::Node & in, OmGroup & g );
    friend QDataStream &operator<<(QDataStream & out, const OmGroup & g );
    friend QDataStream &operator>>(QDataStream & in, OmGroup & g );
};

