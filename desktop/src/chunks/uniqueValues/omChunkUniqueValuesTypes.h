#pragma once

#include "common/omCommon.h"

class ChunkUniqueValues {
private:
    const om::shared_ptr<uint32_t> segIDsPtr_;
    const size_t size_;
    uint32_t const*const segIDs_;

public:
    ChunkUniqueValues(om::shared_ptr<uint32_t> segIDsPtr, const size_t size)
        : segIDsPtr_(segIDsPtr)
        , size_(size)
        , segIDs_(segIDsPtr_.get())
    {}

    inline size_t size() const {
        return size_;
    }

    inline const uint32_t& operator[](const size_t i) const {
        return segIDs_[i];
    }

    inline uint32_t const* begin() const {
        return segIDs_;
    }

    inline uint32_t const* end() const {
        return segIDs_ + size_;
    }

    inline bool contains(const uint32_t val) const
    {
        if(end() == find(val)){
            return false;
        }
        return true;
    }

    // assume we are sorted
    inline uint32_t const* find(const uint32_t val) const
    {
        uint32_t const* iter = std::lower_bound(begin(), end(), val);

        if(iter == end() || *iter != val){
            return end();
        }

        return iter;
    }
};

