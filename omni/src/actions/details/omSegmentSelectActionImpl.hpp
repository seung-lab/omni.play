#ifndef OM_SEGMENT_SELECT_ACTION_IMPL_HPP
#define OM_SEGMENT_SELECT_ACTION_IMPL_HPP

#include "common/omCommon.h"
#include "project/omProject.h"
#include "project/omProject.h"
#include "utility/dataWrappers.h"
#include "system/omEvents.h"

class OmSegmentSelectActionImpl {
private:
	OmID mSegmentationId;
	OmSegIDsSet mNewSelectedIdSet;
	OmSegIDsSet mOldSelectedIdSet;
	OmID mSegmentJustSelectedID;
	void * mSender;
	const std::string mComment;
	const bool mDoScroll;
	const bool mAddToRecentList;

public:
	OmSegmentSelectActionImpl(const OmID segmentationId,
							  const OmSegIDsSet & newSelectedIdSet,
							  const OmSegIDsSet & oldSelectedIdSet,
							  const OmID segmentJustSelected,
							  void* sender,
							  const std::string & comment,
							  const bool doScroll,
							  const bool addToRecentList)
		: mSegmentationId(segmentationId)
		, mNewSelectedIdSet(newSelectedIdSet)
		, mOldSelectedIdSet(oldSelectedIdSet)
		, mSegmentJustSelectedID(segmentJustSelected)
		, mSender(sender)
		, mComment(comment)
		, mDoScroll(doScroll)
		, mAddToRecentList(addToRecentList)
	{}

	void Execute()
	{
		SegmentationDataWrapper sdw(mSegmentationId);
		sdw.GetSegmentCache()->UpdateSegmentSelection(mNewSelectedIdSet,
													  mAddToRecentList);

		OmEvents::SegmentModified(mSegmentationId,
								  mSegmentJustSelectedID,
								  mSender,
								  mComment,
								  mDoScroll);
	}

	void Undo()
	{
		SegmentationDataWrapper sdw(mSegmentationId);
		sdw.GetSegmentCache()->UpdateSegmentSelection(mOldSelectedIdSet,
													  mAddToRecentList);

		OmEvents::SegmentModified(mSegmentationId,
								  mSegmentJustSelectedID,
								  mSender,
								  mComment,
								  mDoScroll);

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
	template <typename T> friend class OmActionLoggerFSThread;
	friend QDataStream &operator<<(QDataStream&, const OmSegmentSelectActionImpl&);
	friend QDataStream &operator>>(QDataStream&, OmSegmentSelectActionImpl&);
};

#endif
