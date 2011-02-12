#ifndef _OM_GROUP_H_
#define _OM_GROUP_H_

#include "common/omCommon.h"
#include "system/omManageableObject.h"
#include "utility/omColorUtils.hpp"

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

	virtual ~OmGroup(){}

	void AddIds(const OmSegIDsSet& ids)
	{
		foreach(OmSegID segid, ids) {
			mIDs.insert(segid);
		}
	}
	void RemoveIds(const OmSegIDsSet& ids)
	{
		foreach(OmSegID segid, ids) {
			mIDs.erase(segid);
		}
	}

	OmGroupName GetName() {
		return mName;
	}

	const OmSegIDsSet& GetIDs() {
		return mIDs;
	}

private:
	OmSegIDsSet mIDs;
	OmColor mColor;

	OmGroupName mName;
	friend class OmGroups;

	friend QDataStream &operator<<(QDataStream & out, const OmGroup & g );
	friend QDataStream &operator>>(QDataStream & in, OmGroup & g );

};

#endif
