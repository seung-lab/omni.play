#ifndef OM_FIND_COMMON_EDGE_HPP
#define OM_FIND_COMMON_EDGE_HPP

#include "segment/omSegments.h"

class OmFindCommonEdge {
private:
    static OmSegmentEdge makeEdge()
    {
        return OmSegmentEdge();
    }

    static OmSegmentEdge makeEdge(OmSegment* childSeg)
    {
        return OmSegmentEdge(childSeg->parent_->value(),
                             childSeg->value(),
                             childSeg->threshold_);
    }

    static OmSegmentEdge makeEdge(OmSegment* parentSeg, OmSegment* childSeg,
                                  const double t)
    {
        return OmSegmentEdge(parentSeg->value(),
                             childSeg->value(),
                             t);
    }

public:
    static OmSegmentEdge FindClosestCommonEdge(OmSegments* segments,
                                               OmSegment* seg1,
                                               OmSegment* seg2)
    {
        if(!seg1 || !seg2){
            return makeEdge();
        }

        if( segments->findRoot(seg1) != segments->findRoot(seg2) ){
            printf("can't split disconnected objects\n");
            return makeEdge();
        }
        if( seg1 == seg2 ){
            printf("can't split object from self\n");
            return makeEdge();
        }

        OmSegment * s1 = seg1;
        while (0 != s1->parent_)
        {
            if(s1->parent_ == seg2) {
                //debug(split, "splitting child from a direct parent\n");
                return makeEdge(s1);
            }
            s1 = s1->parent_;
        }

        OmSegment * s2 = seg2;
        while (0 != s2->parent_)
        {
            if(s2->parent_ == seg1) {
                //debug(split, "splitting child from a direct parent\n");
                return makeEdge(s2);
            }
            s2 = s2->parent_;
        }

        OmSegment * nearestCommonPred = 0;

        OmSegment* one;

        for (one = seg1;
             one != NULL;
             one = one->parent_)
        {
            OmSegment* two;
            for (two = seg2;
                 two != NULL && one != two;
                 two = two->parent_)
            {
            }

            if (one == two) {
                nearestCommonPred = one;
                break;
            }
        }

        assert(nearestCommonPred != 0);

        double minThresh = 100.0;
        OmSegment * minChild = 0;
        for (one = seg1;
             one != nearestCommonPred;
             one = one->parent_)
        {
            if (one->threshold_ < minThresh) {
                minThresh = one->threshold_;
                minChild = one;
            }
        }

        for (one = seg2;
             one != nearestCommonPred;
             one = one->parent_)
        {
            if (one->threshold_ < minThresh) {
                minThresh = one->threshold_;
                minChild = one;
            }
        }

        assert(minChild != 0);
        return makeEdge(minChild);
    }
};

#endif
