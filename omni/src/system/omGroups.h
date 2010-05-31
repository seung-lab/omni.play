#ifndef _OM_GROUPS_H_
#define _OM_GROUPS_H_

#include "system/omManageableObject.h"
#include "system/omGroup.h"


#include <QVector>
class OmSegmentation;

class OmGroups {
public:
        OmGroups(OmSegmentation * seg);
        ~OmGroups();

	OmId AddGroup(OmIDsSet & segids);

protected:
        OmGroups(const OmGroups&);
        OmGroups& operator= (const OmGroups&);

private:
	OmSegmentation * mSegmentation;

        QVector< OmGroup* > mGroups;
};

#endif

