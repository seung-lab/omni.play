#include <QtGui>
#include "chanInspector.h"

#include "common/omStd.h"
#include "volume/omVolume.h"

#include <QThread>
#include <qtconcurrentrun.h>
#include "common/omDebug.h"
#include "utility/sortHelpers.h"
#include "system/buildVolumes.h"

ChanInspector::ChanInspector(ChannelDataWrapper incoming_cdw, QWidget * parent) : QWidget(parent)
{
	setupUi(this);

	cdw = incoming_cdw;
	directoryEdit->setReadOnly(true);

	populateChannelInspector();
}

ChannelDataWrapper ChanInspector::getChannelDataWrapper()
{
	return cdw;
}

void ChanInspector::on_nameEdit_editingFinished()
{
	OmVolume::GetChannel(cdw.getID()).SetName(nameEdit->text().toStdString());
}

void ChanInspector::on_browseButton_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
							"", QFileDialog::ShowDirsOnly);
	if (dir != "") {
		directoryEdit->setText(dir);
		updateFileList();
	}
}

void ChanInspector::on_exportButton_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Export As"));
	if (fileName == NULL){
		return;
	}

	OmVolume::GetChannel(cdw.getID()).ExportInternalData( fileName );
}
QDir ChanInspector::getDir()
{	
	QString regex = patternEdit->text();
	QDir dir( directoryEdit->text() );

	dir.setSorting( QDir::Name );
	dir.setFilter( QDir::Files );
	QStringList filters;
	filters << regex;
	dir.setNameFilters( filters );

	return dir;
}

QStringList ChanInspector::getFileList()
{
	QStringList files = getDir().entryList();
	return SortHelpers::sortNaturally( files );
}

QFileInfoList ChanInspector::getFileInfoList()
{
	QFileInfoList files = getDir().entryInfoList();
	return SortHelpers::sortNaturally( files );
}

void ChanInspector::updateFileList()
{
	listWidget->clear();

	foreach (QString fn, getFileList() ) {
		listWidget->addItem( fn );
	}

	listWidget->update();
}

void ChanInspector::on_patternEdit_textChanged()
{
	updateFileList();
}

void ChanInspector::intermediate_build_call(OmChannel * current_channel)
{
	BuildVolumes bv( current_channel );
	bv.setFileNamesAndPaths( getFileInfoList() );
	bv.build_channel();
}

void ChanInspector::on_buildButton_clicked()
{
	OmChannel & current_channel = OmVolume::GetChannel(cdw.getID());

	intermediate_build_call( &current_channel);
}

void ChanInspector::on_notesEdit_textChanged()
{
	OmVolume::GetChannel(cdw.getID()).SetNote(notesEdit->toPlainText().toStdString());
}

void ChanInspector::populateChannelInspector()
{
	nameEdit->setText( cdw.getName() );
	nameEdit->setMinimumWidth(200);

	// TODO: fix me!
	//const string & my_directory = current_channel.GetSourceDirectoryPath();
	//channelInspectorWidget->directoryEdit->setText(QString::fromStdString(my_directory));

	directoryEdit->setMinimumWidth(200);

	patternEdit->setText( "*" );
	patternEdit->setMinimumWidth(200);

	notesEdit->setPlainText( cdw.getNote() );

	updateFileList();
}

