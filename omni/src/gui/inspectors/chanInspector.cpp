#include <QtGui>
#include "chanInspector.h"

#include "common/omStd.h"
#include "volume/omVolume.h"

#include <QThread>
#include <qtconcurrentrun.h>
#include "common/omDebug.h"

#define DEBUG 0

ChanInspector::ChanInspector(ChannelDataWrapper incoming_cdw, QWidget * parent) : QWidget(parent)
{
	setupUi(this);

	cdw = incoming_cdw;
	directoryEdit->setReadOnly(true);
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

		if (!dir.endsWith("/"))
			dir += QString("/");

		directoryEdit->setText(dir);

		OmVolume::GetChannel(cdw.getID()).SetSourceDirectoryPath(dir.toStdString());

		listWidget->clear();
		list < string >::const_iterator match_it;
		const list < string > &matches = OmVolume::GetChannel(cdw.getID()).GetSourceFilenameRegexMatches();

		for (match_it = matches.begin(); match_it != matches.end(); ++match_it) {
			listWidget->addItem(QString::fromStdString(*match_it));
		}
	}
}

void ChanInspector::on_exportButton_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Export As"));
	if (fileName == NULL)
		return;

	OmVolume::GetChannel(cdw.getID()).ExportInternalData( fileName );
}

void ChanInspector::on_patternEdit_editingFinished()
{
	OmVolume::GetChannel(cdw.getID()).SetSourceFilenameRegex(patternEdit->text().toStdString());
}

void ChanInspector::on_patternEdit_textChanged()
{
	OmVolume::GetChannel(cdw.getID()).SetSourceFilenameRegex(patternEdit->text().toStdString());

	listWidget->clear();
	list < string >::const_iterator match_it;
	const list < string > &matches = OmVolume::GetChannel(cdw.getID()).GetSourceFilenameRegexMatches();

	for (match_it = matches.begin(); match_it != matches.end(); ++match_it) {
		listWidget->addItem(QString::fromStdString(*match_it));
	}
}

void intermediate_build_call(OmChannel * current_channel)
{
	current_channel->BuildVolumeData();
}

void ChanInspector::on_buildButton_clicked()
{
	OmChannel & current_channel = OmVolume::GetChannel(cdw.getID());

	extern void intermediate_build_call(OmChannel * current_channel);
	QFuture < void >future = QtConcurrent::run(intermediate_build_call, &current_channel);
}

void ChanInspector::on_notesEdit_textChanged()
{
	OmVolume::GetChannel(cdw.getID()).SetNote(notesEdit->toPlainText().toStdString());
}
