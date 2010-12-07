#ifndef SORTED_ROOT_SEGMENTS_TYPES_H
#define SORTED_ROOT_SEGMENTS_TYPES_H

#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>

namespace om {
	namespace sortedRootSegments_ {

		struct Node {
			OmSegID segID;
			uint64_t size;

			bool operator ==(const Node& b) const {
				return segID == b.segID && size == b.size;
			}

			friend std::ostream& operator<<(std::ostream &out, const Node &n) {
				out << "[ segID: " << n.segID
					<< ", size: " << n.size
					<< " ]";
				return out;
			}
		};

		// sort size descending; if size match, make segment ids increasing
		static bool cmpNode(const Node& a, const Node& b)
		{
			if(a.size == b.size){
				return a.segID < b.segID;
			}
			return a.size > b.size;
		}

		struct CompareNode : std::binary_function <Node,Node,bool> {
			bool operator() (const Node& x, const Node& y) const
			{return cmpNode(x,y);}
		};

		typedef boost::bimap<
			boost::bimaps::multiset_of<Node,
									   CompareNode>,
			boost::bimaps::unordered_set_of<OmSegID>
			> bm_type;
	}
}
#endif
