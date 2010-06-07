#ifndef _OM_GROUP_H_
#define _OM_GROUP_H_

#include "common/omCommon.h"

class OmGroup : boost::noncopyable {
public:
        OmGroup();
        OmGroup(const OmSegIDsList & segids);
        ~OmGroup();

        void AddIds( const OmSegIDsList & segids);

private:
	OmSegIDsList mIDs;
	OmColor mColor;

	OmColor GetRandomColor();

};

#endif
