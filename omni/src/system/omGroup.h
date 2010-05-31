#ifndef _OM_GROUP_H_
#define _OM_GROUP_H_

#include "common/omCommon.h"

class OmGroup {
public:
        OmGroup();
        OmGroup(OmIDsSet & segids);
        ~OmGroup();

        void AddIds(OmIDsSet & segids);

protected:
	OmColor GetRandomColor();

        OmGroup(const OmGroup&);
        OmGroup& operator= (const OmGroup&);

private:
	OmIDsSet mIDs;
	OmColor mColor;

};

#endif
