#ifndef OM_SEGMENT_LIST_CONTAINER_HPP
#define OM_SEGMENT_LIST_CONTAINER_HPP

class OmSegment;
class OmSegIDsListWithPage;

template <typename T>
class OmSegmentListContainer {
public:
	void insertSegment(OmSegment* seg){
		list_.InsertSegment(seg);
	}

	void clear(){
		list_.Clear();
	}

	size_t size(){
		return list_.Size();
	}

	void updateFromJoin(OmSegment* root, OmSegment* child){
		list_.UpdateFromJoin(root, child);
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	getPageOfSegmentIDs(const uint32_t offset,
						const int numToGet,
						const OmSegID startSeg){
		return list_.GetPageOfSegmentIDs(offset, numToGet, startSeg);
	}

	void updateFromSplit(OmSegment* root, OmSegment* child,
						 const uint64_t newChildSize){
		list_.UpdateFromSplit(root, child, newChildSize);
	}

	void swapSegment(OmSegment* seg, OmSegmentListContainer<T>& two ){
		T::SwapSegment(seg, list_, two.list_);
	}

	uint64_t getSegmentSize(OmSegment* seg){
		return list_.GetSegmentSize(seg);
	}

	OmSegID GetNextSegmentIDinList(const OmSegID id){
		return list_.GetNextSegmentIDinList(id);
	}

	bool IsSegmentContained(const OmSegID id){
		return list_.IsSegmentContained(id);
	}

private:
	T list_;
};

#endif
