#ifndef OM_ACTION_REPLAYER_HPP
#define OM_ACTION_REPLAYER_HPP

#include "common/omCommon.h"

#include <QFileInfo>

namespace om {
	namespace actionReplayer_ {
		enum ActionTypes {
			OmSegmentValidateAction,
			OmSegmentSplitAction,
			OmSegmentJoinAction,
			OmSegmentSelectAction,
			OmSegmentGroupAction,
			OmSegmentUncertainAction,
			OmVolxelSetvalueAction,
			OmSegmentationThresholdChangeAction,
			OmProjectCloseAction,
			OmProjectSaveAction,
		};
	}
}

class OmActionReplayer {
public:
	static void Replay();

private:
	std::map<QString, om::actionReplayer_::ActionTypes> actionStrToType_;

	OmActionReplayer();

	void registerActionTypes();
	void replayFile(const QFileInfo& file);
	void doReplay();

	void dispatchAction(const QString& actionName, QDataStream& in);

	template <typename ACTION, typename IMPL>
	void doReplayFile(QDataStream& in)
	{
		boost::shared_ptr<IMPL> impl(new IMPL());
		in >> (*impl);
		ACTION* action = new ACTION(impl);
		action->Replay();
	}
};

#endif
