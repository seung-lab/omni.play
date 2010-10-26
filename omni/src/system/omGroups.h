#ifndef _OM_GROUPS_H_
#define _OM_GROUPS_H_

#include "common/omCommon.h"
#include "system/omGenericManager.h"
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

	OmID getSegmentationID();
	void populateGroupsList();


private:
	void setGroupIDs(const OmSegIDsSet & set, OmGroup * group, bool doSet);

	OmSegmentation * mSegmentation;
	OmGenericManager<OmGroup> mGroupManager;
	QHash<OmGroupName, OmGroupID> mGroupsByName;

	friend QDataStream &operator<<(QDataStream & out, const OmGroups &);
	friend QDataStream &operator>>(QDataStream & in, OmGroups &);
};

#endif

