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
                   std::vector<std::set<uint32_t> >& results)
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

    boost::unordered_map<uint32_t, std::set<uint32_t> > newSeedSets;

    FOR_EACH( it, allowed ) {
        newSeedSets[sets.find_set(*it)].insert(*it);
    }

    FOR_EACH( it, newSeedSets ) {
        results.push_back(it->second);
    }
}

void get_seeds(std::vector<std::map<int32_t, int32_t> >& seeds,
               const volume::volume& taskVolume,
               const std::set<int32_t>& selected,
               const volume::volume& adjacentVolume)
{
    std::cout << "Getting Seeds" << std::endl
              << taskVolume.Uri() << std::endl
              << adjacentVolume.Uri() << std::endl;
    FOR_EACH(it, selected) {
        std::cout << *it << ", ";
    }

    std::cout << std::endl;

    const int FALSE_OBJ_SIZE_THR=125;

    coords::globalBbox overlap = taskVolume.Bounds();
    overlap.intersect(adjacentVolume.Bounds());

    boost::unordered_map<uint32_t, int> mappingCounts;
    boost::unordered_map<uint32_t, int> sizes;

    // Find corresponding Segments in adjacent volume
    for(int x = overlap.getMin().x; x < overlap.getMax().x; x++)
    {
        for(int y = overlap.getMin().y; y < overlap.getMax().y; y++)
        {
            for(int z = overlap.getMin().z; z < overlap.getMax().z; z++)
            {
                const coords::global point(x, y, z);

                uint32_t taskSegId = taskVolume.GetSegId(point);
            	uint32_t adjacentSegId = adjacentVolume.GetSegId(point);

                if(adjacentSegId > 0) {
            		++sizes[adjacentSegId];

                	if (selected.count(taskSegId)) {
                        ++mappingCounts[adjacentSegId];
                    }
                }
            }
        }
    }

    std::set<uint32_t> correspondingIds;
    FOR_EACH(seg, mappingCounts) {
        correspondingIds.insert(seg->first);
    }

    std::vector<std::set<uint32_t> > adjacentSeeds;

    // group all the segments based on adjacency
    connectedSets(overlap, adjacentVolume, correspondingIds, adjacentSeeds);

    // Prune small segments but don't totally destroy tasks
    FOR_EACH(seed, adjacentSeeds)
	{
		std::vector<uint32_t> toRemove;
    	FOR_EACH(seg, *seed)
    	{
		    if (mappingCounts[*seg] < FALSE_OBJ_SIZE_THR) {
		    	toRemove.push_back(*seg);
		    }
    	}

    	FOR_EACH(seg, toRemove)
    	{
    		if(seed->size() <= 1) {
    			break;
    		}

			seed->erase(*seg);
    	}
    }


    FOR_EACH(seed, adjacentSeeds)
    {
        std::map<int32_t, int32_t> seedMap;
        FOR_EACH(seg, *seed) {
			seedMap[*seg] = sizes[*seg];
        }
		seeds.push_back(seedMap);
    }

}

}} // namespace om::handler
