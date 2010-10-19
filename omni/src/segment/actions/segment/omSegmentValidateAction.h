#ifndef OM_SEGMENT_VALIDATE_ACTION_H
#define OM_SEGMENT_VALIDATE_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"
#include "common/omCommon.h"

class OmSegmentValidateAction : public OmAction {

public:
	OmSegmentValidateAction( const OmID segmentationId,
							 const OmSegIDsSet & selectedSegmentIdsSet,
							 const bool valid);

private:
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string&);
	QString classNameForLogFile(){return "OmSegmentValidateAction";}

	OmID mSegmentationId;
	bool valid_;
	OmSegIDsSet mSelectedSegmentIds;

	template <typename T> friend class OmActionLoggerFSThread;

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentValidateAction & action );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentValidateAction & action );
};

#endif
