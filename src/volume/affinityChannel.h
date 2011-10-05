#pragma once

#include "volume/channel.h"

class affinityChannel {
private:
	OmChannel *const chan_;
	const affinityGraph affinity_;

public:
	affinityChannel(OmChannel* chan, const affinityGraph aff)
		: chan_(chan)
		, affinity_(aff)
	{}

	affinityGraph GetAffinity() const {
		return affinity_;
	}

	OmChannel* Channel() {
		return chan_;
	}

	virtual QString GetDefaultHDF5DatasetName(){
		return "affGraphSingle";
	}
};

