#include "segment/lists/omSegmentListsTypes.hpp"

om::shared_ptr<GUIPageOfSegments>
om::segLists::getPage(const std::vector<SegInfo> list,
                      const uint32_t startIndex,
                      const uint32_t numToGet)
{
    om::shared_ptr<GUIPageOfSegments> ret =
        om::make_shared<GUIPageOfSegments>();

    if(list.empty()){
        return ret;
    }

    if(startIndex > list.size()){
        //std::cout << "The start index is " << startIndex << '\n' << "The size of the list is " << list.size() << '\n';
        assert(startIndex && "illegal page request");
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
