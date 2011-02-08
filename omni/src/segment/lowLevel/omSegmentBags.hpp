#ifndef OM_SEGMENT_BAGS_HPP
#define OM_SEGMENT_BAGS_HPP

#include "common/omCommon.h"
#include "common/omSet.hpp"
#include "segment/omSegment.h"
#include "zi/omUtility.h"

class OmSegmentBags {
private:
    typedef std::list<OmSegment*> set_t;

    std::vector<set_t> bags_;

public:
    OmSegmentBags(const size_t size){
        bags_.resize(size);
    }

    void Resize(const size_t size){
        bags_.resize(size);
    }

    void Join(OmSegment* parent, OmSegment* child)
    {
        const OmSegID pID = parent->value();
        const OmSegID cID = child->value();

        set_t& parentBag = bags_[pID];
        set_t& childBag  = bags_[cID];

        parentBag.push_back(child);

        if(childBag.empty()){
            return;
        }

        parentBag.splice(parentBag.end(), childBag);
    }

    void Split(OmSegment* parent, OmSegment* child)
    {
        const OmSegID pID = parent->value();
        const OmSegID cID = child->value();

        set_t& parentBag = bags_[pID];
        set_t& childBag  = bags_[cID];

        collectChildren(child, childBag);

        FOR_EACH(iter, childBag){
            if(!parentBag.erase(*iter)){
                std::cout << "missing segment " << *iter << "\n";
            }
            childBag.insert(*iter);
        }
    }

private:
    void collectChildren(OmSegment* rootSeg, set_t& set)
    {
        set_t segs;
        segs.push_back(rootSeg);

        while(!segs.empty()){
            OmSegment* segRet = segs.back();
            segs.pop_back();

            FOR_EACH(iter, segRet->getChildren()){
                segs.push_back(*iter);
            }

            set.push_back(segRet);
        }
    }
};

#endif
