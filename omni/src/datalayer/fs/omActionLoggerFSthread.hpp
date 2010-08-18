#ifndef OM_ACTION_LOGGER_THREAD_H
#define OM_ACTION_LOGGER_THREAD_H

#include "common/omCommon.h"
#include "project/omProject.h"
#include "project/omProjectSaveAction.h"
#include "system/omStateManager.h"
#include "utility/OmThreadPool.hpp"

#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QFile>

static const int Omni_Log_Version = 1;
static const QString Omni_Postfix("OMNI_LOG");

template <typename T>
class OmActionLoggerFSThread : public zi::Runnable {
public:
	OmActionLoggerFSThread(T * action, const std::string & str)
		: action_(action), str_(str) {}

	void run();

private:
	T *const action_;
	const std::string str_;
	QDir mLogFolder;

	void setupLogDir();
	QString getFileNameAndPath(const QString & actionName );
};

template <typename T>
void OmActionLoggerFSThread<T>::run()
{
	setupLogDir();

	QFile file(getFileNameAndPath(action_->classNameForLogFile()));
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);

	out << Omni_Log_Version;
	out << (*action_);
	out << Omni_Postfix;

	//	printf("wrote log file %s\n", qPrintable(file.fileName()));
}

template <typename T>
QString OmActionLoggerFSThread<T>::getFileNameAndPath(const QString & actionName)
{
	const QDateTime curDT = QDateTime::currentDateTime();
	const QString date = curDT.toString("yyyy.MM") + curDT.toString("MMM.dd");
	const QString time = curDT.toString("hh.mm.ss.zzz");
	const QString omniFN = OmProject::GetFileName().replace(".omni", "");

	const QString fn = date+"--"+time+"--"+omniFN+"--"+actionName+".log";

	return mLogFolder.filePath(fn);
}

// TODO: move to OmProject, so check is only done once?
template <typename T>
void OmActionLoggerFSThread<T>::setupLogDir()
{
	QString omniFolderName = ".omni";
	QString homeFolder = QDir::homePath();
	QString omniFolderPath = homeFolder + "/"
		+ omniFolderName + "/"
		+ "logFiles" + "/";

	QDir dir = QDir( omniFolderPath );
	if( dir.exists() ){
		mLogFolder = dir;
		return;
	}

	if( QDir::home().mkdir( omniFolderPath ) ){
		printf("made folder %s\n", qPrintable(omniFolderPath) );
		mLogFolder = dir;
	} else {
		const string errMsg = "could not make folder "+omniFolderPath.toStdString() + "\n";
		throw OmIoException(errMsg);
	}
}

#endif
