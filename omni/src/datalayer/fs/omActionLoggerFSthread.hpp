#ifndef OM_ACTION_LOGGER_THREAD_H
#define OM_ACTION_LOGGER_THREAD_H

#include "common/omCommon.h"
#include <zi/threads>

template <typename T>
class OmActionLoggerFSThread : public zi::Runnable {
public:
	OmActionLoggerFSThread(T * action, const std::string & str)
		: action_(action), str_(str) {}

	void run();

private:
	T *const action_;
	const std::string str_;

	QString getFileNameAndPath(const QString & actionName );
};

#endif
