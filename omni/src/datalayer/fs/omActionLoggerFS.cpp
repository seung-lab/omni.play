#include "datalayer/fs/omActionLoggerFS.h"
#include "project/omProject.h"
#include "segment/actions/segment/omSegmentGroupAction.h"
#include "segment/actions/segment/omSegmentJoinAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/segment/omSegmentSplitAction.h"
#include "segment/actions/segment/omSegmentValidateAction.h"
#include "segment/actions/voxel/omVoxelSelectionAction.h"
#include "segment/actions/voxel/omVoxelSelectionSetAction.h"
#include "segment/actions/voxel/omVoxelSetAction.h"
#include "segment/actions/voxel/omVoxelSetConnectedAction.h"
#include "segment/actions/voxel/omVoxelSetValueAction.h"

#include <QDateTime>
#include <QFile>

static const int Omni_Log_Version = 1;
static const QString Omni_Postfix("OMNI_LOG");

OmActionLoggerFS::OmActionLoggerFS()
{
}

OmActionLoggerFS::~OmActionLoggerFS()
{
}

QString OmActionLoggerFS::getFileNameAndPath()
{
	QDateTime curDT = QDateTime::currentDateTime();
	QString time = curDT.toString("yyyy-MM") +
		curDT.toString("MMM-dd.hh-mm-ss-zzz");
	QString newending = "."+time+".log";

	QString fn = OmProject::GetFileName().replace(".omni", newending);
	return mLogFolder.filePath(fn);
}

void OmActionLoggerFS::setupLogDir()
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

	if( QDir::home().mkdir( omniFolderName ) ){
		printf("made folder %s\n", qPrintable(omniFolderPath) );
		mLogFolder = dir;
	} else {
		const string errMsg = "could not make folder "+omniFolderName.toStdString() + "\n";
		throw OmIoException(errMsg);
	}

}

void OmActionLoggerFS::doSave(OmAction * action)
{
	setupLogDir();

	QFile file(getFileNameAndPath());
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);

	out << Omni_Log_Version;
	
	out << Omni_Postfix;
}

void OmActionLoggerFS::save(OmSegmentSplitAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmSegmentGroupAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmSegmentJoinAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmSegmentSelectAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmSegmentValidateAction* action, const std::string &)
{
}


void OmActionLoggerFS::save(OmVoxelSelectionAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmEditSelectionApplyAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmVoxelSetAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmVoxelSetConnectedAction* action, const std::string &)
{
}

void OmActionLoggerFS::save(OmVoxelSetValueAction* action, const std::string &)
{
}

