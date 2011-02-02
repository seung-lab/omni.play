#ifndef OM_SEGMENT_SELECT_ACTION_IMPL_HPP
#define OM_SEGMENT_SELECT_ACTION_IMPL_HPP

#include "utility/omCopyFirstN.hpp"
#include "common/omCommon.h"
#include "common/omString.hpp"
#include "project/omProject.h"
#include "project/omProject.h"
#include "utility/dataWrappers.h"
#include "system/omEvents.h"

class OmSegmentSelectActionImpl {
private:
	SegmentDataWrapper sdw_;
	OmSegIDsSet mNewSelectedIdSet;
	OmSegIDsSet mOldSelectedIdSet;
	void * mSender;
	std::string mComment;
	bool mDoScroll;
	bool mAddToRecentList;
	bool mCenter;
	bool augmentListOnly_;

public:
	OmSegmentSelectActionImpl() {}
	OmSegmentSelectActionImpl(const SegmentDataWrapper& sdw,
							  const OmSegIDsSet & newSelectedIdSet,
							  const OmSegIDsSet & oldSelectedIdSet,
							  void* sender,
							  const std::string & comment,
							  const bool doScroll,
							  const bool addToRecentList,
							  const bool center,
							  const bool augmentListOnly)
		: sdw_(sdw)
		, mNewSelectedIdSet(newSelectedIdSet)
		, mOldSelectedIdSet(oldSelectedIdSet)
		, mSender(sender)
		, mComment(comment)
		, mDoScroll(doScroll)
		, mAddToRecentList(addToRecentList)
		, mCenter(center)
		, augmentListOnly_(augmentListOnly)
	{}

	void Execute()
	{
		if(augmentListOnly_){
			sdw_.SegmentCache()->AddToSegmentSelection(mNewSelectedIdSet);

		}else{
			sdw_.SegmentCache()->UpdateSegmentSelection(mNewSelectedIdSet,
														mAddToRecentList);
		}

		OmEvents::SegmentModified(sdw_,
								  mSender,
								  mComment,
								  mDoScroll,
								  mCenter);
	}

	void Undo()
	{
		if(augmentListOnly_){
			sdw_.SegmentCache()->RemvoeFromSegmentSelection(mOldSelectedIdSet);

		}else{
			sdw_.SegmentCache()->UpdateSegmentSelection(mOldSelectedIdSet,
														mAddToRecentList);
		}

		OmEvents::SegmentModified(sdw_,
								  mSender,
								  mComment,
								  mDoScroll,
								  mCenter);

	}

	std::string Description() const
	{
		static const size_t max = 5;

		std::vector<OmSegID> segIDs;
		segIDs.reserve(max);

		om::utils::CopyFirstN(mNewSelectedIdSet, segIDs, max);

		const std::string nums = om::string::join(segIDs);

		std::string post = "";
		if(mNewSelectedIdSet.size() > max){
			post = "...";
		}

		return "Selected segments: " + nums + post;
	}

	QString classNameForLogFile() const {
		return "OmSegmentSelectAction";
	}

private:
	template <typename T> friend class OmActionLoggerThread;
	friend QDataStream &operator<<(QDataStream&, const OmSegmentSelectActionImpl&);
	friend QDataStream &operator>>(QDataStream&, OmSegmentSelectActionImpl&);
};

#endif
