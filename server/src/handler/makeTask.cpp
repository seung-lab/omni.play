#include "handler/handler.h"
#include "common/common.h"

#include "volume/volume.h"

#include <vector>
#include <set>
#include <string>
#include "boost/unordered_map.hpp"

#include <zi/disjoint_sets/disjoint_sets.hpp>

namespace om {
namespace handler {

void conditionalJoin(zi::disjoint_sets<uint32_t>& sets, uint32_t id1, uint32_t id2)
{
    uint32_t id1_rep = sets.find_set(id1);
    uint32_t id2_rep = sets.find_set(id2);
    sets.join( id1_rep, id2_rep );
}

template<typename T>
void connectedSets(const coords::globalBbox& bounds,
                   const volume::volume& vol,
                   const T& allowed,
                   std::vector<boost::shared_ptr<std::set<uint32_t> > >& results)
{
    uint32_t max_seg_id = 0;
    FOR_EACH(it, allowed) {
        max_seg_id = std::max(max_seg_id, *it);
    }

    zi::disjoint_sets<uint32_t> sets(max_seg_id+1);

    for(int i = bounds.getMin().x + 1; i < bounds.getMax().x; i++)
    {
        for(int j = bounds.getMin().y + 1; j < bounds.getMax().y; j++)
        {
            for(int k = bounds.getMin().z + 1; k < bounds.getMax().z; k++)
            {
                uint32_t seg_id  = vol.GetSegId( coords::global(i,j,k) );
                if (allowed.count(seg_id))
                {
                    uint32_t seg_id1 = vol.GetSegId( coords::global(i-1,j,k) );
                    if ( allowed.count(seg_id1) ) {
                        conditionalJoin(sets, seg_id, seg_id1);
                    }
                    uint32_t seg_id2 = vol.GetSegId( coords::global(i,j-1,k) );
                    if ( allowed.count(seg_id2) ) {
                        conditionalJoin(sets, seg_id, seg_id2);
                    }
                    uint32_t seg_id3 = vol.GetSegId( coords::global(i,j,k-1) );
                    if ( allowed.count(seg_id3) ) {
                        conditionalJoin(sets, seg_id, seg_id3);
                    }
                }
            }
        }
    }

    boost::unordered_map<uint32_t, boost::shared_ptr<std::set<uint32_t> > > newSeedSets;

    FOR_EACH( it, allowed ) {
        uint32_t set = sets.find_set(*it);
        if(!newSeedSets[set]) {
            newSeedSets[set] = boost::make_shared<std::set<uint32_t> >();
        }
        newSeedSets[sets.find_set(*it)]->insert(*it);
    }

    FOR_EACH( it, newSeedSets ) {
        results.push_back(it->second);
    }
}

struct Plane {
    size_t depth;
    common::viewType view;
};

typedef std::set<uint32_t> SegBundle;
typedef std::pair<boost::shared_ptr<SegBundle>, boost::shared_ptr<SegBundle> > BundlePair;

struct Overlap
{
	Overlap(const volume::volume& pre, const std::set<int32_t>& preSegs, const volume::volume& post)
		: pre_(pre)
		, post_(post)
	{
	    bounds_ = pre_.Bounds();
	    bounds_.intersect(post_.Bounds());
        if(bounds_.isEmpty()) {
            return;
        }
        Plane plane = GetExitPlane(post_.Bounds());

	    // Find corresponding Segments in adjacent volume
	    for(int x = bounds_.getMin().x; x < bounds_.getMax().x; x++)
	    {
	        for(int y = bounds_.getMin().y; y < bounds_.getMax().y; y++)
	        {
	            for(int z = bounds_.getMin().z; z < bounds_.getMax().z; z++)
	            {
	                const coords::global point(x, y, z);

	                uint32_t preSegId = pre_.GetSegId(point);
	            	uint32_t postSegId = post_.GetSegId(point);

	                if(postSegId > 0) {
	            		++sizes_[postSegId];

	                	if (preSegs.count(preSegId)) {
	                		postPreCorrespondence_[postSegId].insert(preSegId);
	                        ++mappingCounts_[postSegId];
	                    }
	                }

                    if(plane.view == common::XY_VIEW && z == plane.depth) {
                        exitSegIds_.insert(preSegId);
                    }
                    if(plane.view == common::XZ_VIEW && y == plane.depth) {
                        exitSegIds_.insert(preSegId);
                    }
                    if(plane.view == common::ZY_VIEW && x == plane.depth) {
                        exitSegIds_.insert(preSegId);
                    }
	            }
	        }
	    }
	}

    Plane GetExitPlane(const coords::globalBbox& post)
    {
        if(bounds_.getMin().x > post.getMin().x) {
            Plane p = { bounds_.getMin().x, common::ZY_VIEW };
            return p;
        }
        if(bounds_.getMax().x < post.getMax().x) {
            Plane p = { bounds_.getMax().x, common::ZY_VIEW };
            return p;
        }
        if(bounds_.getMin().y > post.getMin().y) {
            Plane p = { bounds_.getMin().y, common::XZ_VIEW };
            return p;
        }
        if(bounds_.getMax().y < post.getMax().y) {
            Plane p = { bounds_.getMax().y, common::XZ_VIEW };
            return p;
        }
        if(bounds_.getMin().z > post.getMin().z) {
            Plane p = { bounds_.getMin().z, common::XY_VIEW };
            return p;
        }
        if(bounds_.getMax().z < post.getMax().z) {
            Plane p = { bounds_.getMax().z, common::XY_VIEW };
            return p;
        }
    }

	std::vector<BundlePair> GetBundles()
	{
		std::vector<BundlePair> ret;

	    std::set<uint32_t> postIds;
	    FOR_EACH(seg, mappingCounts_) {
	        postIds.insert(seg->first);
	    }

		std::vector<boost::shared_ptr<SegBundle> > postBundles;

	    // group all the segments based on adjacency
	    connectedSets(bounds_, post_, postIds, postBundles);

	    FOR_EACH(postBundle, postBundles)
	    {
			boost::shared_ptr<SegBundle> preBundle = boost::make_shared<SegBundle>();
			FOR_EACH(seg, **postBundle) {
				preBundle->insert(postPreCorrespondence_[*seg].begin(), postPreCorrespondence_[*seg].end());
			}
			ret.push_back(BundlePair(preBundle, *postBundle));
	    }

		return ret;
	}

    bool Leaves(const SegBundle& bundle)
    {
        FOR_EACH(segId, bundle) {
            if (exitSegIds_.count(*segId)) {
                return true;
            }
        }
        return false;
    }

	std::map<int32_t, int32_t> MakeSeed(const SegBundle& bundle)
	{
		const double FALSE_OBJ_RATIO_THR=0.5;
		std::map<int32_t, int32_t> ret;
		uint32_t largest = 0;
        uint32_t largestSize = 0;
		FOR_EACH(seg, bundle)
    	{
    		if (((double)mappingCounts_[*seg]) / ((double)sizes_[*seg]) >= FALSE_OBJ_RATIO_THR) {
		    	ret[*seg] = sizes_[*seg];
		    }
		    if(sizes_[*seg] > largestSize) {
		    	largest = *seg; largestSize = sizes_[*seg];
		    }
    	}

    	if(ret.size() == 0) {
    		ret[largest] = largestSize;
    	}

    	return ret;
	}

    bool IsEmpty() const {
        return bounds_.isEmpty();
    }

private:
	const volume::volume& pre_;
	const volume::volume& post_;
	coords::globalBbox bounds_;
	boost::unordered_map<uint32_t, int> mappingCounts_;
    boost::unordered_map<uint32_t, int> sizes_;
    boost::unordered_map<uint32_t, std::set<uint32_t> > postPreCorrespondence_;
    std::set<uint32_t> exitSegIds_;
};


void get_seeds(std::vector<std::map<int32_t, int32_t> >& seeds,
               const volume::volume& taskVolume,
               const std::set<int32_t>& selected,
               const volume::volume& adjacentVolume)
{
    std::cout << "Getting Seeds" << std::endl
              << taskVolume.Uri() << std::endl
              << adjacentVolume.Uri() << std::endl;
    Overlap overlap(taskVolume, selected, adjacentVolume);

    if(overlap.IsEmpty()) {
        std::cout << "Regions do not overlap." << std::endl;
        return;
    }

    std::vector<BundlePair> bundles = overlap.GetBundles();

    int i = 0;

    FOR_EACH(bundle, bundles)
    {
        std::cout << "Bundle " << i++ << ":" << std::endl;

        std::cout << "Pre Segments: ";
        FOR_EACH(seg, *bundle->first) {
            std::cout << *seg << ",";
        }
        std::cout << std::endl;

        std::cout << "Post Segments: ";
        FOR_EACH(seg, *bundle->second) {
            std::cout << *seg << ",";
        }
        std::cout << std::endl;

        if(!overlap.Leaves(*bundle->first)) {
            std::cout << "Pre-side Bundle does not enter the post volume." << std::endl << std::endl;
            continue;
        }

        seeds.push_back(overlap.MakeSeed(*bundle->second));
        std::cout << "Spawned!" << std::endl << std::endl;
    }
}

}} // namespace om::handler
