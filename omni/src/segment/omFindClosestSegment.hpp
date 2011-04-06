#ifndef OM_FIND_CLOSEST_SEGMENT_HPP
#define OM_FIND_CLOSEST_SEGMENT_HPP

#include "utility/omThreadPoolBatched.hpp"

class OmFindClosestSegment {
private:
    OmSegment *const joinee_;
    const DataBbox joineeBoundingBox_;
    const Vector3i joineeCenter_;

    typedef OmThreadPoolBatched<OmSegment*,
                                OmFindClosestSegment,
                                VectorArgPolicy> taskpool_t;


public:
    OmFindClosestSegment(OmSegment* joinee)
        : joinee_(joinee)
        , joineeBoundingBox_(joinee->BoundingBox())
    {}

    void FindClosestByEuclideanDistance(OmSegment* big)
    {
        taskpool_t pool;
        pool.Start(&OmFindClosestSegment::findContaingOrClosestSegments,
                   this);

        std::deque<OmSegment*> segs;
        segs.push_back(big);

        while(!segs.empty())
        {
            OmSegment* segRet = segs.back();
            segs.pop_back();

            FOR_EACH(iter, segRet->GetChildren()){
                segs.push_back(*iter);
            }

            pool.AddOrSpawnTasks(segRet);
        }

        pool.JoinPool();
    }

private:
    void findContaingOrClosestSegments(const std::vector<OmSegment*>& segs)
    {
        std::deque<OmSegment*> containsJoinee;

        boost::optional<OmSegment*> closestToJoinee;
        boost::optional<double> distanceSquaredToJoinee;

        FOR_EACH(iter, segs)
        {
            OmSegment* seg = *iter;

            const DataBbox box = seg->BoundingBox();

            if(box.contains(joineeBoundingBox_))
            {
                containsJoinee.push_back(seg);
                continue;
            }

            const Vector3i center =
            double distanceSquared =


        }

    }

    // struct SegPair {
    //     OmSegment* joinee;
    //     OmSegment* joinedInto;
    // };

    // boost::share_ptr<std::deque<SegPair> > getPairsOfSegmentsBeingJoined()
    // {
    //     OmSegments* segments = sdw_.Segments();
    //     OmSegmentLists* segmentLists = sdw_.SegmentLists();

    //     const OmSegIDsSet selected = segments->GetSelectedSegmentIds();

    //     std::multimap<int64_t, OmSegment*> bySize;

    //     FOR_EACH(iter, selected)
    //     {
    //         OmSegment* seg = segments->GetSegment(*iter);
    //         const int64_t numVoxels = segmentLists->GetSizeWithChildren(*iter);
    //         bySize.insert(std::make_pair(numVoxels, seg));
    //     }

    //     boost::share_ptr<std::deque<SegPair> > ret =
    //         boost::make_shared<std::deque<SegPair> >();

    //     if(bySize.size() < 2){
    //         return ret;
    //     }

    //     std::vector<OmSegment*> segs;
    //     segs.reserve(bySize.size());
    //     FOR_EACH(iter, bySize){
    //         segs.push_back(iter->second);
    //     }

    //     for(int i = i; i < segs.size(); ++i)
    //     {
    //         SegPair pair = { segs[0], segs[i] };
    //         ret->push_back(pair);
    //     }

    //     return ret;
    // }
};

#endif
