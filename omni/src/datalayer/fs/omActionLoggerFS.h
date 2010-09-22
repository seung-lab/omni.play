#ifndef OM_ACTION_LOGGER_H
#define OM_ACTION_LOGGER_H

#include "datalayer/fs/omActionLoggerFSthread.hpp"
#include "project/omProject.h"

#include <QDir>
#include <zi/mutex>
#include <zi/utility>

class OmSegmentSplitAction;
class OmSegmentGroupAction;
class OmSegmentJoinAction;
class OmSegmentSelectAction;
class OmSegmentValidateAction;
class OmProjectSaveAction;

class OmActionLoggerFS {
public:
	template <class T> static void save(T * action, const std::string &);

private:
	bool initialized;
	zi::Mutex mutex_;
	QDir mLogFolder;

	OmActionLoggerFS();
	~OmActionLoggerFS();
	void setupLogDir();
	QDir& doGetLogFolder();

	static inline OmActionLoggerFS & Instance(){
		return zi::Singleton<OmActionLoggerFS>::Instance();
	}

	friend class zi::Singleton<OmActionLoggerFS>;
};

template <class T>
void OmActionLoggerFS::save(T * action, const std::string & str)
{
	boost::shared_ptr<OmActionLoggerFSThread<T> >
		task(new OmActionLoggerFSThread<T>(action, str,
										   Instance().doGetLogFolder()));

	task->run(); //QT may delete *action before we have a chance to save it!
	//	OmProject::GetGlobalThreadPool().addTaskBack(task);
}

QDataStream &operator<<(QDataStream & out, const OmSegmentSplitAction & a );
QDataStream &operator>>(QDataStream & in,  OmSegmentSplitAction & a );

QDataStream &operator<<(QDataStream & out, const OmSegmentGroupAction & a );
QDataStream &operator>>(QDataStream & in,  OmSegmentGroupAction & a );

QDataStream &operator<<(QDataStream & out, const OmSegmentJoinAction & a );
QDataStream &operator>>(QDataStream & in,  OmSegmentJoinAction & a );

QDataStream &operator<<(QDataStream & out, const OmSegmentSelectAction & a );
QDataStream &operator>>(QDataStream & in,  OmSegmentSelectAction & a );

QDataStream &operator<<(QDataStream & out, const OmSegmentValidateAction & a );
QDataStream &operator>>(QDataStream & in,  OmSegmentValidateAction & a );

QDataStream &operator<<(QDataStream & out, const OmProjectSaveAction & a );
QDataStream &operator>>(QDataStream & in,   OmProjectSaveAction& a );

#endif
