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
                   volume::volume& vol,
                   const T& allowed,
                   std::vector<std::set<int32_t> >& results)
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

    boost::unordered_map<uint32_t, std::set<int32_t> > newSeedSets;

    FOR_EACH( it, allowed ) {
        newSeedSets[sets.find_set(*it)].insert(*it);
    }

    FOR_EACH( it, newSeedSets ) {
        results.push_back(it->second);
    }
}

void get_seeds(std::vector<std::set<int32_t> >& seeds,
               volume::volume& taskVolume,
               const std::set<int32_t>& selected,
               volume::volume& adjacentVolume)
{
    std::cout << "Getting Seeds" << std::endl;
    FOR_EACH(it, selected) {
        std::cout << *it << ", ";
    }

    std::cout << std::endl;

    const int DUST_SIZE_THR_2D=25;
    const int FALSE_OBJ_SIZE_THR=125;

    coords::globalBbox overlap = taskVolume.CoordSystem().GetDataExtent();
    overlap.intersect(adjacentVolume.CoordSystem().GetDataExtent());

    std::cout << "\tOverlap:\t\t" << overlap << std::endl;

    boost::unordered_set<uint32_t> intersectingSegIds;
    // Find intersecting segIds
    FOR_EACH(it, selected)
    {
        const uint32_t& segId = *it;
        segment::data segData = taskVolume.GetSegmentData(segId);

        // object too small
        if (segData.size < DUST_SIZE_THR_2D) {
            continue;
        }

        coords::globalBbox segOverlap = segData.bounds;
        segOverlap.offset(taskVolume.CoordSystem().GetDataExtent().getMin());
        segOverlap.intersect(overlap);

        // Does not overlap with boundary region
        if (!segOverlap.isEmpty()) {
            intersectingSegIds.insert(segId);
        }
    }

    std::cout << "Found " << intersectingSegIds.size()
              << " segments in the overlapping region." << std::endl;
    FOR_EACH(it, intersectingSegIds) {
        std::cout << *it << ", ";
    }

    std::cout << std::endl;

    std::vector<std::set<int32_t> > groupedSegIds;
    // group all the segments based on adjacency
    connectedSets(overlap, taskVolume, intersectingSegIds, groupedSegIds);

    std::cout << "Separated them out into " << groupedSegIds.size()
              << " groups of segment ids." << std::endl;

    std::vector<boost::unordered_map<uint32_t, int> > mappingCounts(groupedSegIds.size());

    // Find corresponding Segments in adjacent volume
    for(int x = overlap.getMin().x; x < overlap.getMax().x; x++)
    {
        for(int y = overlap.getMin().y; y < overlap.getMax().y; y++)
        {
            for(int z = overlap.getMin().z; z < overlap.getMax().z; z++)
            {
                const coords::global point(x, y, z);

                uint32_t taskSegId = taskVolume.GetSegId(point);

                for(int i = 0; i < mappingCounts.size(); i++)
                {
                    if ( groupedSegIds[i].count(taskSegId) ) {
                        uint32_t adjacentSegId = adjacentVolume.GetSegId(point);
                        ++mappingCounts[i][adjacentSegId];
                        break;
                    }
                }
            }
        }
    }

    // Find all the segIds in the adjacent volume with enough overlap
    for(int i = 0; i < mappingCounts.size(); i++)
    {
        std::set<int32_t> correspondingIds;
        FOR_EACH(seg, mappingCounts[i]) {
            if (seg->second >= FALSE_OBJ_SIZE_THR) {
                correspondingIds.insert(seg->first);
            }
        }
        seeds.push_back(correspondingIds);
    }

}

}} // namespace om::handler
