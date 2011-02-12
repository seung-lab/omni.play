#ifndef OM_HPP
#define OM_HPP

namespace om {
	enum ZeroMem {
		ZERO_FILL,
		DONT_ZERO_FILL
	};
	enum Blocking {
		BLOCKING,
		NON_BLOCKING
	};
	enum ShouldUpdateAsType{
		UPDATE_AS_TYPE,
		DONT_UPDATE_AS_TYPE
	};
	enum SetValid{
		SET_VALID,
		SET_NOT_VALID
	};
	enum ShouldThrottle {
		THROTTLE,
		DONT_THROTTLE
	};
	enum ShouldFifo {
		FIFO,
		DONT_FIFO
	};
	enum AffinityGraph {
		NO_AFFINITY,
		X_AFFINITY,
		Y_AFFINITY,
		Z_AFFINITY
	};
	enum ShouldBufferWrites {
		BUFFER_WRITES,
		DONT_BUFFER_WRITES
	};
	enum AllowOverwrite {
		WRITE_ONCE,
		OVERWRITE
	};
}

#endif
