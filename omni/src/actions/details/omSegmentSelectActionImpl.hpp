#ifndef OM_SEGMENT_SELECT_ACTION_IMPL_HPP
#define OM_SEGMENT_SELECT_ACTION_IMPL_HPP

#include "common/omCommon.h"
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

public:
	OmSegmentSelectActionImpl() {}
	OmSegmentSelectActionImpl(const SegmentDataWrapper& sdw,
							  const OmSegIDsSet & newSelectedIdSet,
							  const OmSegIDsSet & oldSelectedIdSet,
							  void* sender,
							  const std::string & comment,
							  const bool doScroll,
							  const bool addToRecentList,
							  const bool center)
		: sdw_(sdw)
		, mNewSelectedIdSet(newSelectedIdSet)
		, mOldSelectedIdSet(oldSelectedIdSet)
		, mSender(sender)
		, mComment(comment)
		, mDoScroll(doScroll)
		, mAddToRecentList(addToRecentList)
		, mCenter(center)
	{}

	void Execute()
	{
		sdw_.GetSegmentCache()->UpdateSegmentSelection(mNewSelectedIdSet,
													   mAddToRecentList);

		OmEvents::SegmentModified(sdw_,
								  mSender,
								  mComment,
								  mDoScroll,
								  mCenter);
	}

	void Undo()
	{
		sdw_.GetSegmentCache()->UpdateSegmentSelection(mOldSelectedIdSet,
													   mAddToRecentList);

		OmEvents::SegmentModified(sdw_,
								  mSender,
								  mComment,
								  mDoScroll,
								  mCenter);

	}

	std::string Description() const
	{
		QStringList segs;
		foreach(const OmSegID& segID, mNewSelectedIdSet){
			segs << QString::number(segID);
		}

		const QString lineItem = "Selected segments: " + segs.join(", ");
		return lineItem.toStdString();
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
