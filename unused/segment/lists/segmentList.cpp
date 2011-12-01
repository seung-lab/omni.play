#include "segment/lists/segmentListByMRU.hpp"
#include "segment/lists/segmentListForGUI.hpp"
#include "segment/lists/segmentListGlobal.hpp"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "zi/mutex.h"
#include "zi/utility.h"
#include "segment/lists/segmentLists.h"
#include "segment/lists/segmentListLowLevel.hpp"

segmentLists::segmentLists()
    : segmentListsLL_(new segmentListLowLevel(this))
    , globalList_(new segmentListGlobal())
    , working_(new segmentListForGUI(om::WORKING))
    , uncertain_(new segmentListForGUI(om::UNCERTAIN))
    , valid_(new segmentListForGUI(om::VALID))
    , recent_(new segmentListByMRU(this))
{}

segmentLists::~segmentLists()
{}

void segmentLists::TouchRecent(segment* seg)
{
    // recent is internally locked
    recent_->Touch(seg);
}

size_t segmentLists::SizeRecent()
{
    // recent is internally locked
    return recent_->Size();
}

boost::shared_ptr<GUIPageOfSegments>
segmentLists::GetSegmentGUIPageRecent(const GUIPageRequest& request)
{
    // recent is internally locked
    return recent_->GetSegmentGUIPage(request);
}

void segmentLists::Swap(boost::shared_ptr<segmentListForGUI>& list)
{
    zi::rwmutex::write_guard g(lock_);

    switch(list->Type()){
    case om::VALID:
        valid_ = list;
        break;
    case om::WORKING:
        working_ = list;
        break;
    case om::UNCERTAIN:
        uncertain_ = list;
        break;
    default:
        throw common::argException("unknown type");
    }
}

void segmentLists::Swap(boost::shared_ptr<segmentListGlobal>& globalList)
{
    zi::rwmutex::write_guard g(lock_);
    globalList_ = globalList;
}

size_t segmentLists::Size(const om::SegListType type)
{
    zi::rwmutex::read_guard g(lock_);
    return get(type)->Size();
}

uint64_t segmentLists::GetNumTopLevelSegs()
{
    zi::rwmutex::read_guard g(lock_);
    return working_->Size() + valid_->Size() + uncertain_->Size();
}

int64_t segmentLists::TotalNumVoxels()
{
    zi::rwmutex::read_guard g(lock_);

    return working_->TotalNumVoxels() +
        valid_->TotalNumVoxels() +
        uncertain_->TotalNumVoxels();
}

int64_t segmentLists::NumVoxels(const om::SegListType type)
{
    zi::rwmutex::read_guard g(lock_);
    return get(type)->TotalNumVoxels();
}

boost::shared_ptr<GUIPageOfSegments>
segmentLists::GetSegmentGUIPage(const om::SegListType type,
                                  const GUIPageRequest& request)
{
    zi::rwmutex::read_guard g(lock_);
    return get(type)->GetSegmentGUIPage(request);
}

int64_t segmentLists::GetSizeWithChildren(const common::segId segID)
{
    zi::rwmutex::read_guard g(lock_);
    return globalList_->GetSizeWithChildren(segID);
}

int64_t segmentLists::GetSizeWithChildren(segment* seg)
{
    zi::rwmutex::read_guard g(lock_);
    return globalList_->GetSizeWithChildren(seg);
}

common::segId segmentLists::GetNextSegIDinWorkingList(const SegmentDataWrapper& sdw)
{
    zi::rwmutex::read_guard g(lock_);

    if(!sdw.IsSegmentValid()){
        return 0;
    }

    boost::optional<SegInfo> segInfo = working_->GetNextSegID(sdw.GetSegmentID());
    if(!segInfo){
        return 0;
    }
    return segInfo->segID;
}

// choose largest segment
common::segId segmentLists::GetNextSegIDinWorkingList(const SegmentationDataWrapper& sdw)
{
    zi::rwmutex::read_guard g(lock_);

    if(!sdw.IsSegmentationValid()){
        return 0;
    }

    const common::segIdSet selectedIDs = sdw.GetSelectedSegmentIDs();
    std::vector<SegInfo> infos;
    infos.reserve(selectedIDs.size());

    FOR_EACH(iter, selectedIDs)
    {
        boost::optional<SegInfo> segInfo = working_->Get(*iter);
        if(!segInfo){
            continue;
        }
        infos.push_back(*segInfo);
    }

    if(infos.empty()){
        return 0;
    }

    // largest segInfo is the first segInfo
    const SegInfo largest =
        *zi::min_element(infos.begin(), infos.end(), om::segLists::CmpSegInfo);

    return largest.segID;
}

void segmentLists::RefreshGUIlists(){
    segmentListsLL_->ForceRefreshGUIlists();
}

int64_t segmentLists::GetNumChildren(const common::segId segID)
{
    zi::rwmutex::read_guard g(lock_);
    return globalList_->GetNumChildren(segID);
}

int64_t segmentLists::GetNumChildren(segment* seg)
{
    zi::rwmutex::read_guard g(lock_);
    return globalList_->GetNumChildren(seg);
}
