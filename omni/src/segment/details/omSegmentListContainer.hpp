#ifndef OM_SEGMENT_LIST_CONTAINER_HPP
#define OM_SEGMENT_LIST_CONTAINER_HPP

class OmSegment;

template <typename T>
class OmSegmentListContainer {
public:
	void insertSegment(OmSegment* seg){
		list.insertSegment(seg);
	}

	void clear(){
		list.clear();
	}

	size_t size(){
		return list.size();
	}

	void updateFromJoin(OmSegment* root, OmSegment* child){
		list.updateFromJoin(root, child);
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	getPageOfSegmentIDs(const uint32_t offset,
							  const int numToGet,
							  const OmSegID startSeg){
		return list.getPageOfSegmentIDs(offset, numToGet, startSeg);
	}

	void updateFromSplit(OmSegment* root, OmSegment* child,
						 const uint64_t newChildSize){
		list.updateFromSplit(root, child, newChildSize);
	}

	void swapSegment(OmSegment* seg, OmSegmentListContainer<T>& two ){
		T::swapSegment(seg, list, two.list);
	}

	uint64_t getSegmentSize(OmSegment* seg){
		return list.getSegmentSize(seg);
	}

private:
	T list;
};

#endif
