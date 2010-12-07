#include "utility/dataWrappers.h"
#include <QtGui>
#include "project/omProject.h"
#include "chanInspector.h"
#include "volInspector.h"
#include "common/omStd.h"
#include "volume/omVolume.h"
#include "common/omDebug.h"
#include "utility/sortHelpers.h"
#include "volume/build/omBuildChannel.hpp"
#include "system/omProjectData.h"

#include <boost/make_shared.hpp>

ChanInspector::ChanInspector(ChannelDataWrapper incoming_cdw, QWidget * parent)
	: QWidget(parent)
{
	setupUi(this);

	cdw = boost::make_shared<ChannelDataWrapper>(incoming_cdw);
	directoryEdit->setReadOnly(true);

	populateChannelInspector();

	//	connect(nameEdit, SIGNAL(editingFinished()),
	//		this, SLOT(nameEditChanged()), Qt::DirectConnection);
}

ChannelDataWrapper ChanInspector::getChannelDataWrapper()
{
	return *cdw;
}

void ChanInspector::on_nameEdit_editingFinished()
{
	OmProject::GetChannel(cdw->getID()).SetCustomName(nameEdit->text());
}

void ChanInspector::on_browseButton_clicked()
{
	const QString dir =
		QFileDialog::getExistingDirectory(this, "Choose Directory",
						  "", QFileDialog::ShowDirsOnly);
	if (dir != "") {
		directoryEdit->setText(dir);
		updateFileList();
	}
}

void ChanInspector::on_exportButton_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Export As"));
	if( NULL == fileName ){
		return;
	}

	OmProject::GetChannel(cdw->getID()).ExportInternalData(fileName, false);
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

QStringList ChanInspector::getFileList(){
	return SortHelpers::SortNaturally(getDir().entryList());
}

QFileInfoList ChanInspector::getFileInfoList(){
	return SortHelpers::SortNaturally(getDir().entryInfoList());
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

void ChanInspector::on_buildButton_clicked()
{
	OmChannel & current_channel = OmProject::GetChannel(cdw->getID());

	OmBuildChannel * bc = new OmBuildChannel( &current_channel );
	bc->setFileNamesAndPaths( getFileInfoList() );
	bc->BuildNonBlocking();
}

void ChanInspector::on_notesEdit_textChanged()
{
	OmProject::GetChannel(cdw->getID()).SetNote(notesEdit->toPlainText());
}

OmID ChanInspector::getChannelID()
{
	return cdw->getID();
}

void ChanInspector::populateChannelInspector()
{
	nameEdit->setText( cdw->getName() );
	nameEdit->setMinimumWidth(200);

	//TODO: fix me!
	if( 0 ){
		// use path from where import files were orginally...
	} else {
		directoryEdit->setText( OmProjectData::getAbsolutePath() );
	}
	directoryEdit->setMinimumWidth(200);

	patternEdit->setText( "*" );
	patternEdit->setMinimumWidth(200);


	OmChannel & current_channel = OmProject::GetChannel(cdw->getID());
	gridLayout_3->addWidget(new OmVolInspector(current_channel, this), 4, 0, 1, 1);

	notesEdit->setPlainText( cdw->getNote() );

	updateFileList();
}

void ChanInspector::nameEditChanged()
{
	//	sdw.setName( nameEdit->text() );
}
