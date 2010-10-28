#ifndef OM_SEGMENT_LIST_CONTAINER_HPP
#define OM_SEGMENT_LIST_CONTAINER_HPP

class OmSegment;

template <typename T>
class OmSegmentListContainer {
public:
	void insertSegment(OmSegment* seg){
		list_.insertSegment(seg);
	}

	void clear(){
		list_.clear();
	}

	size_t size(){
		return list_.size();
	}

	void updateFromJoin(OmSegment* root, OmSegment* child){
		list_.updateFromJoin(root, child);
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	getPageOfSegmentIDs(const uint32_t offset,
							  const int numToGet,
							  const OmSegID startSeg){
		return list_.getPageOfSegmentIDs(offset, numToGet, startSeg);
	}

	void updateFromSplit(OmSegment* root, OmSegment* child,
						 const uint64_t newChildSize){
		list_.updateFromSplit(root, child, newChildSize);
	}

	void swapSegment(OmSegment* seg, OmSegmentListContainer<T>& two ){
		T::swapSegment(seg, list_, two.list_);
	}

	uint64_t getSegmentSize(OmSegment* seg){
		return list_.getSegmentSize(seg);
	}

	OmSegID GetNextSegmentIDinList(const OmSegID id){
		return list_.GetNextSegmentIDinList(id);
	}

private:
	T list_;
};

#endif
