#ifndef OM_ACTION_LOGGER_H
#define OM_ACTION_LOGGER_H

#include "actions/io/omActionLoggerFSthread.hpp"
#include "project/omProject.h"
#include "utility/omThreadPool.hpp"
#include "zi/omMutex.h"
#include "zi/omUtility.h"

#include <QDir>

class OmSegmentSplitActionImpl;
class OmSegmentGroupActionImpl;
class OmSegmentJoinActionImpl;
class OmSegmentSelectActionImpl;
class OmSegmentValidateActionImpl;
class OmSegmentUncertainActionImpl;
class OmProjectSaveActionImpl;
class OmProjectCloseActionImpl;

class OmActionLoggerFS : private om::singletonBase<OmActionLoggerFS> {
public:
	template <typename T>
	static void save(boost::shared_ptr<T> actionImpl,
					 const std::string& str);

private:
	bool initialized;
	zi::mutex mutex_;
	QDir mLogFolder;

	OmThreadPool threadPool_;

	OmActionLoggerFS()
		: initialized(false)
	{
		threadPool_.start(1);
	}

	~OmActionLoggerFS(){
		threadPool_.stop();
	}

	void setupLogDir();
	QDir& doGetLogFolder();

	friend class zi::singleton<OmActionLoggerFS>;
};

template <typename T>
void OmActionLoggerFS::save(boost::shared_ptr<T> actionImpl,
							const std::string& str)
{
	boost::shared_ptr<OmActionLoggerFSThread<T> >
		task(new OmActionLoggerFSThread<T>(actionImpl,
										   str,
										   instance().doGetLogFolder()));

	instance().threadPool_.addTaskBack(task);
}

QDataStream& operator<<(QDataStream& out, const OmSegmentSplitActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmSegmentSplitActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmSegmentGroupActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmSegmentGroupActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmSegmentJoinActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmSegmentJoinActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmSegmentSelectActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmSegmentSelectActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmSegmentValidateActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmSegmentValidateActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmSegmentUncertainActionImpl&);
QDataStream& operator>>(QDataStream& in, OmSegmentUncertainActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmProjectSaveActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmProjectSaveActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmProjectCloseActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmProjectCloseActionImpl&);

#endif
