#ifndef OM_SEGMENT_VALIDATE_ACTION_H
#define OM_SEGMENT_VALIDATE_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"
#include "common/omCommon.h"

class OmSegment;
class SegmentDataWrapper;
class SegmentationDataWrapper;

class OmSegmentValidateAction : public OmAction {

public:
	static void Validate(const SegmentDataWrapper& sdw,
						 const bool valid);

	static void Validate(const SegmentationDataWrapper& sdw,
						 const bool valid);

private:
	OmSegmentValidateAction(const OmID segmentationId,
							boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
							const bool valid);
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string&);
	QString classNameForLogFile(){return "OmSegmentValidateAction";}

	OmID mSegmentationId;
	bool valid_;
	boost::shared_ptr<std::set<OmSegment*> > selectedSegments_;

	template <typename T> friend class OmActionLoggerFSThread;

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentValidateAction & action );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentValidateAction & action );
};

#endif
