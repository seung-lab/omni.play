#include "datalayer/fs/omActionLoggerFS.h"
#include "datalayer/archive/omDataArchiveBoost.h"
#include "project/omProject.h"
#include "project/omProjectSaveAction.h"
#include "system/omStateManager.h"

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

OmActionLoggerFS::OmActionLoggerFS()
{
}

OmActionLoggerFS::~OmActionLoggerFS()
{
}

QString OmActionLoggerFS::getFileNameAndPath(const QString & actionName)
{
	const QDateTime curDT = QDateTime::currentDateTime();
	const QString date = curDT.toString("yyyy.MM") + curDT.toString("MMM.dd");
	const QString time = curDT.toString("hh.mm.ss.zzz");
	const QString omniFN = OmProject::GetFileName().replace(".omni", "");

	const QString fn = date+"--"+time+"--"+omniFN+"--"+actionName+".log";

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

	if( QDir::home().mkdir( omniFolderPath ) ){
		printf("made folder %s\n", qPrintable(omniFolderPath) );
		mLogFolder = dir;
	} else {
		const string errMsg = "could not make folder "+omniFolderPath.toStdString() + "\n";
		throw OmIoException(errMsg);
	}

}

QDataStream &operator<<(QDataStream & out, const OmSegmentValidateAction & a)
{
        //meta data
	int version = 1;
	out << version;
        out << a.mSelectedSegmentIds;
        out << a.mCreate;
        out << a.mSegmentationId;

        return out;
}

QDataStream &operator>>(QDataStream & in, OmSegmentValidateAction & a)
{
	int version;
	in >> version;
        in >> a.mSelectedSegmentIds;
        in >> a.mCreate;
        in >> a.mSegmentationId;

	a.SetActivate(false);

        return in;
}

QDataStream &operator<<(QDataStream & out, const QUndoStack & s )
{
	for(int i = 0; i < s.count(); i++) {
		out << s.command(i);
	}

	return out;
}

QDataStream &operator>>(QDataStream & in, const QUndoStack & s )
{
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentSplitAction & a )
{
	return out;
}

QDataStream &operator>>(QDataStream & in,  OmSegmentSplitAction & a )
{
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentGroupAction & a )
{
	return out;
}

QDataStream &operator>>(QDataStream & in,  OmSegmentGroupAction & a )
{
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentJoinAction & a )
{
	return out;
}

QDataStream &operator>>(QDataStream & in,  OmSegmentJoinAction & a )
{
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentSelectAction & a )
{
	return out;
}

QDataStream &operator>>(QDataStream & in,  OmSegmentSelectAction & a )
{
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmVoxelSelectionAction & a )
{
	return out;
}

QDataStream &operator>>(QDataStream & in,  OmVoxelSelectionAction & a )
{
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmEditSelectionApplyAction & a )
{
	return out;
}

QDataStream &operator>>(QDataStream & in,  OmEditSelectionApplyAction & a )
{
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmVoxelSetAction & a )
{
	return out;
}

QDataStream &operator>>(QDataStream & in,  OmVoxelSetAction & a )
{
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmVoxelSetConnectedAction & a )
{
	return out;
}

QDataStream &operator>>(QDataStream & in,   OmVoxelSetConnectedAction& a )
{
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmProjectSaveAction & a )
{
	return out;
}

QDataStream &operator>>(QDataStream & in,   OmProjectSaveAction& a )
{
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmVoxelSetValueAction & a )
{
	return out;
}

QDataStream &operator>>(QDataStream & in,  OmVoxelSetValueAction & a )
{ 
	return in;
}

