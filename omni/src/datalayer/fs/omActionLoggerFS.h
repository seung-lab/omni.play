#ifndef OM_ACTION_LOGGER_H
#define OM_ACTION_LOGGER_H

#include "common/omCommon.h"
#include <QDir>
#include <QDataStream>
static const int Omni_Log_Version = 1;
static const QString Omni_Postfix("OMNI_LOG");

class OmSegmentSplitAction;
class OmSegmentGroupAction;
class OmSegmentJoinAction;
class OmSegmentSelectAction;
class OmSegmentValidateAction;
class OmProjectSaveAction;
class OmAction;

class OmActionLoggerFS {
 public:
	OmActionLoggerFS();
	~OmActionLoggerFS();

	template <class T>
	void save(T * action, const std::string &);

 private:
	QDir mLogFolder;

	void setupLogDir();
	QString getFileNameAndPath(const QString & actionName );
};

template <class T>
void OmActionLoggerFS::save(T * action, const std::string &)
{
	setupLogDir();

	QFile file(getFileNameAndPath(action->classNameForLogFile()));
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);

	out << Omni_Log_Version;
	out << (*action);
	out << Omni_Postfix;
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
