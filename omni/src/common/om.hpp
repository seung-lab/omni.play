#ifndef OM_HPP
#define OM_HPP

namespace om {
	enum zeroMem {
		ZERO_FILL,
		DONT_ZERO_FILL
	};
	enum BlockingRead {
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
}

#endif
