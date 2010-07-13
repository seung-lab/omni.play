#ifndef OM_ACTION_LOGGER_H
#define OM_ACTION_LOGGER_H

#include "common/omCommon.h"

class OmSegmentSplitAction;
class OmSegmentGroupAction;
class OmSegmentJoinAction;
class OmSegmentSelectAction;
class OmSegmentValidateAction;

class OmVoxelSelectionAction;
class OmEditSelectionApplyAction;
class OmVoxelSetAction;
class OmVoxelSetConnectedAction;
class OmVoxelSetValueAction;

class OmActionLoggerFS {
 public:
	static void save(OmSegmentSplitAction*, const std::string &);
	static void save(OmSegmentGroupAction*, const std::string &);
	static void save(OmSegmentJoinAction*, const std::string &);
	static void save(OmSegmentSelectAction*, const std::string &);
	static void save(OmSegmentValidateAction*, const std::string &);

	static void save(OmVoxelSelectionAction*, const std::string &);
	static void save(OmEditSelectionApplyAction*, const std::string &);
	static void save(OmVoxelSetAction*, const std::string &);
	static void save(OmVoxelSetConnectedAction*, const std::string &);
	static void save(OmVoxelSetValueAction*, const std::string &);

 private:

};

#endif
