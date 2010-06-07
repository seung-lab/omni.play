#ifndef _OM_GROUPS_H_
#define _OM_GROUPS_H_

#include "common/omCommon.h"
#include <QVector>

class OmSegmentation;
class OmGroup;

class OmGroups : boost::noncopyable {
public:
        OmGroups(OmSegmentation * seg);
        ~OmGroups();

	OmId AddGroup(OmSegIDsList & segids);

private:
	OmSegmentation * mSegmentation;

        QVector< OmGroup* > mGroups;
};

#endif

