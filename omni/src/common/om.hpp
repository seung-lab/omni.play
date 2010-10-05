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

	enum RewriteSegments{
		REWRITE_SEGMENTS,
		DONT_REWRITE_SEGMENTS
	};

} // namespace Om

#endif
