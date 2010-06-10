#ifndef _OM_GROUP_H_
#define _OM_GROUP_H_

#include "common/omCommon.h"
#include "system/omManageableObject.h"

class OmGroup : public OmManageableObject {
public:
        OmGroup(OmId);
        OmGroup(const OmSegIDsList & segids);
        ~OmGroup();

        void AddIds( const OmSegIDsList & segids);

private:
	OmSegIDsList mIDs;
	OmColor mColor;

	OmColor GetRandomColor();

	OmGroupName mName;
	friend class OmGroups;

	friend QDataStream &operator<<(QDataStream & out, const OmGroup & g );
	friend QDataStream &operator>>(QDataStream & in, OmGroup & g );

};

#endif
