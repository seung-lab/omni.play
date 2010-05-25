#ifndef _OM_GROUPS_H_
#define _OM_GROUPS_H_

#include "system/omManageableObject.h"
#include "system/omGroup.h"
#include "volume/omSegmentation.h"


#include <QVector>

class OmGroups {
public:
        OmGroups(OmSegmentation * seg);
        ~OmGroups();

protected:
        OmGroups(const OmGroups&);
        OmGroups& operator= (const OmGroups&);

private:
	OmSegmentation * mSegmentation;

        QVector< OmGroup > mGroups;
};

#endif

