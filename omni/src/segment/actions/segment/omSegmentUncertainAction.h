#ifndef OM_SEGMENT_UNCERTAIN_ACTION_H
#define OM_SEGMENT_UNCERTAIN_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"
#include "common/omCommon.h"

class OmSegment;
class SegmentDataWrapper;
class SegmentationDataWrapper;

class OmSegmentUncertainAction : public OmAction {

public:
	static void SetUncertain(const SegmentDataWrapper& sdw,
							 const bool uncertain);

	static void SetUncertain(const SegmentationDataWrapper& sdw,
							 const bool uncertain);

private:
	OmSegmentUncertainAction(const OmID segmentationId,
							 boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
							 const bool valid);
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string&);
	QString classNameForLogFile(){return "OmSegmentUncertainAction";}

	OmID mSegmentationId;
	bool uncertain_;
	boost::shared_ptr<std::set<OmSegment*> > selectedSegments_;

	template <typename T> friend class OmActionLoggerFSThread;
	friend QDataStream &operator<<(QDataStream&,
								   const OmSegmentUncertainAction&);
	friend QDataStream &operator>>(QDataStream&,
								   OmSegmentUncertainAction&);
};

#endif
