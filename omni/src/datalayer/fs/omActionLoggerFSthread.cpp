#include "datalayer/fs/omActionLoggerFSthread.hpp"
#include "project/omProject.h"
#include "datalayer/fs/omActionLoggerFS.h"
#include "project/omProjectSaveAction.h"
#include "system/omStateManager.h"

#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QFile>

static const int Omni_Log_Version = 1;
static const QString Omni_Postfix("OMNI_LOG");

template <typename T>
void OmActionLoggerFSThread<T>::run()
{
	zi::Guard g(OmActionLoggerFS::getLock());

	QFile file(getFileNameAndPath(action_->classNameForLogFile()));
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);

	out << Omni_Log_Version;
	out << (*action_);
	out << Omni_Postfix;

	printf("wrote log file %s\n", qPrintable(file.fileName()));
}

template <typename T>
QString OmActionLoggerFSThread<T>::getFileNameAndPath(const QString & actionName)
{
	const QDateTime curDT = QDateTime::currentDateTime();
	const QString date = curDT.toString("yyyy.MM") + curDT.toString("MMM.dd");
	const QString time = curDT.toString("hh.mm.ss.zzz");
	const QString omniFN = OmProject::GetFileName().replace(".omni", "");

	const QString fn = date+"--"+time+"--"+omniFN+"--"+actionName+".log";

	return OmActionLoggerFS::getLogFolder().filePath(fn);
}
