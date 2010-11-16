#ifndef OM_ACTION_LOGGER_HPP
#define OM_ACTION_LOGGER_HPP

#include "actions/io/omActionLoggerTask.hpp"
#include "actions/omActions.hpp"
#include "project/omProject.h"
#include "system/omProjectData.h"
#include "utility/omThreadPool.hpp"
#include "zi/omMutex.h"
#include "zi/omUtility.h"

#include <QDir>

class OmActionLogger : private om::singletonBase<OmActionLogger> {
public:
	static QDir& LogFolder(){
		return instance().logFolder_;
	}

	template <typename T>
	static void save(boost::shared_ptr<T> actionImpl,
					 const std::string& str)
	{
		boost::shared_ptr<OmActionLoggerTask<T> >
			task(new OmActionLoggerTask<T>(actionImpl,
										   str,
										   LogFolder()));

		instance().threadPool_.addTaskBack(task);
	}

private:
	QDir logFolder_;
	OmThreadPool threadPool_;

	OmActionLogger()
	{
		setupLogDir();
		threadPool_.start(1);
	}

	~OmActionLogger(){
		threadPool_.stop();
	}

	void setupLogDir()
	{
		const QDir filesDir = OmProjectData::GetFilesFolderPath();
		const QString logFolderPath =
			filesDir.absolutePath() +
			QDir::separator() +
			"logFiles" +
			QDir::separator();

		logFolder_ = QDir(logFolderPath);
		if(logFolder_.exists()){
			return;
		}

		if(QDir::home().mkdir(logFolderPath)){
			printf("made folder %s\n", qPrintable(logFolderPath));
		} else {
			throw OmIoException("could not make folder", logFolderPath);
		}
	}

	friend class zi::singleton<OmActionLogger>;
};

#endif
