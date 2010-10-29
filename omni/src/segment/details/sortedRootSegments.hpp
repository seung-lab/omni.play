#ifndef SORTED_ROOT_SEGMENTS_HPP
#define SORTED_ROOT_SEGMENTS_HPP

class SortedRootSegments{
private:
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

	static bool cmpNode(const Node& a, const Node& b)
	{
		if(a.size == b.size){
			return a.segID < b.segID;
		}
		return a.size > b.size;
	}

	std::vector<Node> sorted_;

public:
	void Add(const OmSegID segID, const uint64_t size)
	{
		const Node n = { segID, size };

		sorted_.insert(
			std::upper_bound(sorted_.begin(),
							 sorted_.end(),
							 n,
							 cmpNode),
			n);
	}

	void Remove(const OmSegID segID, const uint64_t oldSize)
	{
		const Node n = { segID, oldSize };

		std::vector<Node>::iterator iter
			= std::lower_bound(sorted_.begin(),
							   sorted_.end(),
							   n,
							   cmpNode);

		if(iter == sorted_.end()){
			assert(0);
		}

		assert(*iter == n);

		sorted_.erase(iter);
	}

	void Dump()
	{
		for( size_t i = 0; i < sorted_.size(); ++i){
			std::cout << "seg " << i << ", size " << sorted_[i] << "\n";
		}
	}

	size_t Size() const {
		return sorted_.size();
	}

	void Clear() {
		sorted_.clear();
	}
};

#endif
