#ifndef OM_ACTION_LOGGER_H
#define OM_ACTION_LOGGER_H

#include "common/omCommon.h"
#include <QDir>
#include <QDataStream>

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

class OmProjectSaveAction;

class OmAction;

class OmActionLoggerFS {
 public:
	OmActionLoggerFS();
	~OmActionLoggerFS();

	void save(OmSegmentSplitAction*, const std::string &);
	void save(OmSegmentGroupAction*, const std::string &);
	void save(OmSegmentJoinAction*, const std::string &);
	void save(OmSegmentSelectAction*, const std::string &);
	void save(OmSegmentValidateAction*, const std::string &);

	void save(OmVoxelSelectionAction*, const std::string &);
	void save(OmEditSelectionApplyAction*, const std::string &);
	void save(OmVoxelSetAction*, const std::string &);
	void save(OmVoxelSetConnectedAction*, const std::string &);
	void save(OmVoxelSetValueAction*, const std::string &);

	void save(OmProjectSaveAction*, const std::string &);

 private:
	QDir mLogFolder;

	void setupLogDir();
	QString getFileNameAndPath();
	void doSave(OmAction * action);
};

#endif
