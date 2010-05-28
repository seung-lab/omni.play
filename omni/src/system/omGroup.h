#ifndef _OM_GROUP_H_
#define _OM_GROUP_H_

#include "common/omCommon.h"

class OmGroup {
public:
        OmGroup();
        OmGroup(OmIds & segids);
        ~OmGroup();

        void AddIds(OmIds & segids);

protected:
	OmColor GetRandomColor();

        OmGroup(const OmGroup&);
        OmGroup& operator= (const OmGroup&);

private:
	OmIds mIDs;
	OmColor mColor;

};

#endif
