#ifndef OM_MST_TYPES_H
#define OM_MST_TYPES_H

struct OmMSTEdge {
	uint32_t number;
	uint32_t node1ID;
	uint32_t node2ID;
	double threshold;
	uint8_t userJoin;
	uint8_t userSplit;
	uint8_t wasJoined;	// transient state
};

struct OmMSTImportEdge {
	uint32_t node1ID;
	uint32_t node2ID;
	float threshold;
};

#endif
