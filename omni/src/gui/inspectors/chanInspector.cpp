#include <QtGui>
#include "chanInspector.h"

#include "common/omStd.h"
#include "volume/omVolume.h"

#include <QThread>
#include <qtconcurrentrun.h>
#include "common/omDebug.h"

#define DEBUG 0

ChanInspector::ChanInspector(OmId chan_id, QWidget * parent)
 : QWidget(parent)
{
	setupUi(this);

	my_id = chan_id;
	directoryEdit->setReadOnly(true);
}

void ChanInspector::setId(OmId new_id)
{
	my_id = new_id;
}

void ChanInspector::on_nameEdit_editingFinished()
{
	OmVolume::GetChannel(my_id).SetName(nameEdit->text().toStdString());
}

void ChanInspector::on_browseButton_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
							"", QFileDialog::ShowDirsOnly);
	if (dir != "") {

		if (!dir.endsWith("/"))
			dir += QString("/");

		directoryEdit->setText(dir);

		OmVolume::GetChannel(my_id).SetSourceDirectoryPath(dir.toStdString());

		listWidget->clear();
		list < string >::const_iterator match_it;
		const list < string > &matches = OmVolume::GetChannel(my_id).GetSourceFilenameRegexMatches();

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

	QString fname = fileName.section('/', -1);
	QString dpath = fileName.remove(fname);

	OmVolume::GetChannel(my_id).ExportInternalData(dpath.toStdString(), fname.toStdString());
}

void ChanInspector::on_patternEdit_editingFinished()
{
	OmVolume::GetChannel(my_id).SetSourceFilenameRegex(patternEdit->text().toStdString());
}

void ChanInspector::on_patternEdit_textChanged()
{
	// OmVolume::GetChannel(my_id).SetSourceFilePattern(patternEdit->text().toStdString());

	OmVolume::GetChannel(my_id).SetSourceFilenameRegex(patternEdit->text().toStdString());

	// change list view - listWidget
	//GetSourceFileRegexMatches(list<string> &)
	listWidget->clear();
	list < string >::const_iterator match_it;
	const list < string > &matches = OmVolume::GetChannel(my_id).GetSourceFilenameRegexMatches();

	for (match_it = matches.begin(); match_it != matches.end(); ++match_it) {
		listWidget->addItem(QString::fromStdString(*match_it));
	}

}

void intermediate_build_call(OmChannel * current_channel)
{
	//debug("genone","intermediate build call");

	// OmChannel &current_channel = OmVolume::GetChannel(my_id);
	current_channel->BuildVolumeData();
}

void ChanInspector::on_buildButton_clicked()
{
	//debug("genone","ChanInspector::on_buildButton_clicked");
	// OmVolume::GetChannel(my_id).BuildVolumeData();

	/*
	   // call 'void QImage::invertPixels(InvertMode mode)' in a separate thread
	   QImage image = ...;
	   QFuture<void> future = QtConcurrent::run(image, &QImage::invertPixels, QImage::InvertRgba);
	   ...
	   future.waitForFinished();
	   // At this point, the pixels in 'image' have been inverted
	 */
	OmChannel & current_channel = OmVolume::GetChannel(my_id);
	//current_channel.BuildVolumeData();
	//QFuture<void> future = QtConcurrent::run(current_channel, &OmChannel::BuildVolumeData);

	extern void intermediate_build_call(OmChannel * current_channel);
	QFuture < void >future = QtConcurrent::run(intermediate_build_call, &current_channel);

}

void ChanInspector::on_notesEdit_textChanged()
{
	OmVolume::GetChannel(my_id).SetNote(notesEdit->toPlainText().toStdString());
}

void ChanInspector::setChannelID(const OmId channID)
{
	ChannelID = channID;
}

OmId ChanInspector::getChannelID()
{
	return ChannelID;
}

void ChanInspector::setFilterID(const OmId filterID)
{
	FilterID = filterID;
}

OmId ChanInspector::getFilterID()
{
	return FilterID;
}
