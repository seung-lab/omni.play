#pragma once

#include "common/common.h"
#include "segment/segment.h"
#include "segment/segmentChildrenTypes.h"



class segmentChildren {
private:
    std::vector<segChildCont_t> list_;

public:
    segmentChildren(const size_t size)
    {
        list_.resize(size);
    }

    inline void Resize(const size_t size){
        list_.resize(size);
    }

    inline const segChildCont_t& GetChildren(const common::segId segID) const {
        return list_[segID];
    }

    inline const segChildCont_t& GetChildren(segment* seg) const {
        return list_[seg->value()];
    }

    inline void AddChild(const common::segId segID, segment* child){
        list_[segID].insert(child);
    }

    inline void AddChild(segment* seg, segment* child){
        list_[seg->value()].insert(child);
    }

    inline void RemoveChild(const common::segId segID, segment* child){
        list_[segID].erase(child);
    }

    inline void RemoveChild(segment* seg, segment* child){
        list_[seg->value()].erase(child);
    }
};

