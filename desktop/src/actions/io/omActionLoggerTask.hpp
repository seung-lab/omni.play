#pragma once

#include "actions/io/omActionOperators.h"
#include "project/omProject.h"
#include "utility/omLockedPODs.hpp"
#include "zi/omThreads.h"
#include "zi/omUtility.h"

#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QFile>

static const int Omni_Log_Version = 2;
static const QString Omni_Postfix("OMNI_LOG");

class OmActionLoggerTaskCounter
    : private om::singletonBase<OmActionLoggerTaskCounter> {
public:
    static uint32_t Get(){
        return instance().counter_.inc();
    }

private:
    LockedUint32 counter_;
    friend class zi::singleton<OmActionLoggerTaskCounter>;
};

template <typename T>
class OmActionLoggerTask : public zi::runnable {
public:
    OmActionLoggerTask(std::shared_ptr<T> action,
                       const std::string& doOrUndo,
                       QDir& logFolder)
        : action_(action)
        , doOrUndo_(doOrUndo)
        , logFolder_(logFolder)
    {}

    void run()
    {
        const QString actionName = action_->classNameForLogFile();
        const QString fnp = getFileNameAndPath(actionName);

        QFile file(fnp);
        if(!file.open(QIODevice::WriteOnly)){
            throw om::IoException("could not write");
        }

        QDataStream out(&file);
        out.setByteOrder( QDataStream::LittleEndian );
        out.setVersion(QDataStream::Qt_4_6);

        out << Omni_Log_Version;
        out << actionName;
        out << (*action_);
        out << Omni_Postfix;

        // printf("wrote log file %s\n", qPrintable(file.fileName()));
    }

private:
    const std::shared_ptr<T> action_;
    const std::string doOrUndo_;
    QDir logFolder_;

    QString getFileNameAndPath(const QString& actionName)
    {
        const uint32_t count = OmActionLoggerTaskCounter::Get();

        const QDateTime curDT = QDateTime::currentDateTime();
        const QString date = curDT.toString("yyyy.MM") + curDT.toString("MMM.dd");
        const QString time = curDT.toString("hh.mm.ss.zzz");
        const QString sessionCounter =
            QString("%1").arg(QString::number(count), 9, QChar('0'));
        const QString actionDoOrUndo = QString::fromStdString(doOrUndo_);

        const QString fn =
            date+"--"+
            time+"--"+
            sessionCounter+"--"+
            actionName+"--"+
            actionDoOrUndo+
            ".log";

        return logFolder_.filePath(fn);
    }
};

