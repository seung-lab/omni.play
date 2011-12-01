#include "segment/lists/segmentListsTypes.hpp"

boost::shared_ptr<GUIPageOfSegments>
om::segLists::getPage(const std::vector<SegInfo> list,
                      const uint32_t startIndex,
                      const uint32_t numToGet)
{
    boost::shared_ptr<GUIPageOfSegments> ret =
        boost::make_shared<GUIPageOfSegments>();

    if(list.empty()){
        return ret;
    }

    if(startIndex > list.size()){
        assert(0 && "illegal page request");
    }

    ret->pageNum = startIndex / numToGet;

    uint32_t endIndex = startIndex + numToGet;
    if(endIndex > list.size()){
        endIndex = list.size();
    }

    ret->segs.reserve(endIndex - startIndex);

    for(uint32_t i = startIndex; i < endIndex; ++i){
        ret->segs.push_back(list[i]);
    }

    return ret;
}
