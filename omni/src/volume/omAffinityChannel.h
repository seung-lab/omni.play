#ifndef OM_AFFINITY_CHANNEL_H
#define OM_AFFINITY_CHANNEL_H

#include "volume/omChannel.h"

class OmAffinityChannel {
private:
	OmChannel *const chan_;
	const om::AffinityGraph affinity_;

public:
	OmAffinityChannel(OmChannel* chan, const om::AffinityGraph aff)
		: chan_(chan)
		, affinity_(aff)
	{}

	om::AffinityGraph GetAffinity() const {
		return affinity_;
	}

	OmChannel* Channel() {
		return chan_;
	}

	virtual QString GetDefaultHDF5DatasetName(){
		return "affGraphSingle";
	}
};

#endif
