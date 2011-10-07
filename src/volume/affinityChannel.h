#pragma once

#include "volume/channel.h"

class affinityChannel {
private:
	channel *const chan_;
	const affinityGraph affinity_;

public:
	affinityChannel(channel* chan, const affinityGraph aff)
		: chan_(chan)
		, affinity_(aff)
	{}

	affinityGraph GetAffinity() const {
		return affinity_;
	}

	channel* Channel() {
		return chan_;
	}

	virtual std::string GetDefaultHDF5DatasetName(){
		return "affGraphSingle";
	}
};

