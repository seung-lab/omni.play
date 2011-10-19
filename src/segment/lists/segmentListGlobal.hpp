#pragma once

#include "segment/lists/segmentListsTypes.hpp"

class segmentListGlobal {
private:
    const std::vector<SegInfo> list_;

public:
    segmentListGlobal()
    {}

    segmentListGlobal(const std::vector<SegInfo>& list)
        : list_(list)
    {}

    inline int64_t GetSizeWithChildren(const common::segId segID)
    {
        if(segID >= list_.size()){
            std::cout << "segment " << segID << "not found\n";
            return 0;
        }
        return list_[segID].sizeIncludingChildren;
    }

    inline int64_t GetSizeWithChildren(segment* seg){
        return GetSizeWithChildren(seg->value());
    }

    inline int64_t GetNumChildren(const common::segId segID)
    {
        if(segID >= list_.size()){
            std::cout << "segment " << segID << "not found\n";
            return 0;
        }
        return list_[segID].numChildren;
    }

    inline int64_t GetNumChildren(segment* seg){
        return GetNumChildren(seg->value());
    }

    boost::optional<SegInfo> Get(const common::segId segID)
    {
        if(segID >= list_.size()){
            std::cout << "segment " << segID << "not found\n";
            return boost::optional<SegInfo>();
        }
        return boost::optional<SegInfo>(list_[segID]);
    }
};

