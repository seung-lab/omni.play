#ifndef OM_SEGMENT_BAGS_HPP
#define OM_SEGMENT_BAGS_HPP

#include "common/omCommon.h"
#include "zi/omUtility.h"

class OmSegmentBags {
private:
    typedef std::set<OmSegID> set_t;

    std::vector<set_t> bags_;

public:
    OmSegmentBags(const size_t size){
        bags_.resize(size);
    }

    void Resize(const size_t size){
        bags_.resize(size);
    }

    inline void Join(const OmSegID childRootID, const OmSegID parentRootID)
    {
/*
        if(!bags_.count(childRootID)){
            bags_[parentRootID].insert(childRootID);
            return;
        }

        set_t out;
        const set_t& child = bags_[childRootID];
        const set_t& parent = bags_[parentRootID];

        zi::set_union(child.begin(), child.end(),
                      parent.begin(), parent.end(),
                      std::inserter(out, out.begin()));

        bags_[parentRootID] = out;
        bags_[childRootID].clear();
*/
    }
};

#endif
