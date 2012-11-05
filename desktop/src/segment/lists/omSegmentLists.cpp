#include "segment/lists/omSegmentListByMRU.hpp"
#include "segment/lists/omSegmentListForGUI.hpp"
#include "segment/lists/omSegmentListGlobal.hpp"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "zi/omMutex.h"
#include "zi/omUtility.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/lists/omSegmentListLowLevel.hpp"

OmSegmentLists::OmSegmentLists()
    : segmentListsLL_(new OmSegmentListLowLevel(this))
    , globalList_(new OmSegmentListGlobal())
    , working_(new OmSegmentListForGUI(om::common::WORKING))
    , uncertain_(new OmSegmentListForGUI(om::common::UNCERTAIN))
    , valid_(new OmSegmentListForGUI(om::common::VALID))
    , recent_(new OmSegmentListByMRU(this))
{}

OmSegmentLists::~OmSegmentLists()
{}

void OmSegmentLists::TouchRecent(OmSegment* seg)
{
    // recent is internally locked
    recent_->Touch(seg);
}

size_t OmSegmentLists::SizeRecent()
{
    // recent is internally locked
    return recent_->Size();
}

om::shared_ptr<GUIPageOfSegments>
OmSegmentLists::GetSegmentGUIPageRecent(const GUIPageRequest& request)
{
    // recent is internally locked
    return recent_->GetSegmentGUIPage(request);
}

void OmSegmentLists::Swap(om::shared_ptr<OmSegmentListForGUI>& list)
{
    zi::rwmutex::write_guard g(lock_);

    switch(list->Type()){
    case om::common::VALID:
        valid_ = list;
        break;
    case om::common::WORKING:
        working_ = list;
        break;
    case om::common::UNCERTAIN:
        uncertain_ = list;
        break;
    default:
        throw OmArgException("unknown type");
    }
}

void OmSegmentLists::Swap(om::shared_ptr<OmSegmentListGlobal>& globalList)
{
    zi::rwmutex::write_guard g(lock_);
    globalList_ = globalList;
}

size_t OmSegmentLists::Size(const om::common::SegListType type)
{
    zi::rwmutex::read_guard g(lock_);
    return get(type)->Size();
}

uint64_t OmSegmentLists::GetNumTopLevelSegs()
{
    zi::rwmutex::read_guard g(lock_);
    return working_->Size() + valid_->Size() + uncertain_->Size();
}

int64_t OmSegmentLists::TotalNumVoxels()
{
    zi::rwmutex::read_guard g(lock_);

    return working_->TotalNumVoxels() +
        valid_->TotalNumVoxels() +
        uncertain_->TotalNumVoxels();
}

int64_t OmSegmentLists::NumVoxels(const om::common::SegListType type)
{
    zi::rwmutex::read_guard g(lock_);
    return get(type)->TotalNumVoxels();
}

om::shared_ptr<GUIPageOfSegments>
OmSegmentLists::GetSegmentGUIPage(const om::common::SegListType type,
                                  const GUIPageRequest& request)
{
    zi::rwmutex::read_guard g(lock_);
    return get(type)->GetSegmentGUIPage(request);
}

int64_t OmSegmentLists::GetSizeWithChildren(const OmSegID segID)
{
    zi::rwmutex::read_guard g(lock_);
    return globalList_->GetSizeWithChildren(segID);
}

int64_t OmSegmentLists::GetSizeWithChildren(OmSegment* seg)
{
    zi::rwmutex::read_guard g(lock_);
    return globalList_->GetSizeWithChildren(seg);
}

OmSegID OmSegmentLists::GetNextSegIDinWorkingList(const SegmentDataWrapper& sdw)
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
OmSegID OmSegmentLists::GetNextSegIDinWorkingList(const SegmentationDataWrapper& sdw)
{
    zi::rwmutex::read_guard g(lock_);

    if(!sdw.IsSegmentationValid()){
        return 0;
    }

    const OmSegIDsSet selectedIDs = sdw.GetSelectedSegmentIDs();
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

void OmSegmentLists::RefreshGUIlists(){
    segmentListsLL_->ForceRefreshGUIlists();
}

int64_t OmSegmentLists::GetNumChildren(const OmSegID segID)
{
    zi::rwmutex::read_guard g(lock_);
    return globalList_->GetNumChildren(segID);
}

int64_t OmSegmentLists::GetNumChildren(OmSegment* seg)
{
    zi::rwmutex::read_guard g(lock_);
    return globalList_->GetNumChildren(seg);
}
