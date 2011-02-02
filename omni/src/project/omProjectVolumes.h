#ifndef OM_PROJECT_VOLUMES_HPP
#define OM_PROJECT_VOLUMES_HPP

/*
 *	Manages data structures that are shared between various parts of the system.  Making centralized
 *	changes in the StateManager will send events that cause the other interested systems to be
 *	notified and synchronized.
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/omCommon.h"

class OmAffinityGraphManager;
class OmChannelManager;
class OmSegmentationManager;

class OmProjectVolumes {
public:
	OmProjectVolumes();
	~OmProjectVolumes();

	OmChannelManager& Channels(){
		return *channels_;
	}

	OmSegmentationManager& Segmentations(){
		return *segmentations_;
	}

	OmAffinityGraphManager& AffinityGraphs(){
		return *affGraphs_;
	}

private:
	OmChannelManager *const channels_;
	OmSegmentationManager *const segmentations_;
	OmAffinityGraphManager *const affGraphs_;

	friend QDataStream&operator<<(QDataStream& out, const OmProjectVolumes& p );
	friend QDataStream&operator>>(QDataStream& in, OmProjectVolumes& p );
};

#endif
