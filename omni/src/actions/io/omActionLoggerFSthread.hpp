#ifndef OM_ACTION_LOGGER_THREAD_H
#define OM_ACTION_LOGGER_THREAD_H

#include "project/omProject.h"
#include "utility/omLockedPODs.hpp"

#include "zi/omThreads.h"

#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QFile>

static const int Omni_Log_Version = 1;
static const QString Omni_Postfix("OMNI_LOG");

template <typename T>
class OmActionLoggerFSThread : public zi::runnable {
public:
	OmActionLoggerFSThread(boost::shared_ptr<T> action,
						   const std::string& doOrUndo,
						   QDir& logFolder)
		: action_(action)
		, doOrUndo_(doOrUndo)
		, logFolder_(logFolder)
	{}

	void run()
	{
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

private:
	const boost::shared_ptr<T> action_;
	const std::string doOrUndo_;
	QDir logFolder_;

	QString getFileNameAndPath(const QString& actionName)
	{
		static LockedUint32 counter;
		++counter;

		const QDateTime curDT = QDateTime::currentDateTime();
		const QString date = curDT.toString("yyyy.MM") + curDT.toString("MMM.dd");
		const QString time = curDT.toString("hh.mm.ss.zzz");
		const QString omniFN = OmProject::GetFileName().replace(".omni", "");
		const QString sessionCounter = QString::number(counter.get());
		const QString actionDoOrUndo = QString::fromStdString(doOrUndo_);

		const QString fn =
			date+"--"+
			time+"--"+
			omniFN+"--"+
			sessionCounter+"--"+
			actionName+"--"+
			actionDoOrUndo+
			".log";

		return logFolder_.filePath(fn);
	}
};

#endif
