#ifndef OM_CHUNK_UNIQUE_VALUES_TYPES_HPP
#define OM_CHUNK_UNIQUE_VALUES_TYPES_HPP

#include "common/omCommon.h"

class ChunkUniqueValues {
private:
	const boost::shared_ptr<uint32_t> IDs_;
	const size_t size_;
	uint32_t *const data_;

public:
	ChunkUniqueValues(boost::shared_ptr<uint32_t> IDs, const size_t size)
		: IDs_(IDs)
		, size_(size)
		, data_(IDs.get())
	{}

	size_t size() const {
		return size_;
	}

	const uint32_t& operator[](const size_t i) const {
		return data_[i];
	}

	uint32_t const* begin() const {
		return data_;
	}

	uint32_t const* end() const {
		return data_ + size_;
	}

	bool contains(const uint32_t val) const {
		if(end() == find(val)){
			return false;
		}
		return true;
	}

	// assume we are sorted
	uint32_t const* find(const uint32_t val) const
	{
		uint32_t const* iter = std::lower_bound(begin(), end(), val);

		if(iter == end() || *iter != val){
			return end();
		}

		return iter;
	}
};

#endif
