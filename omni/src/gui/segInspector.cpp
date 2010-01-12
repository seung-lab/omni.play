#include <QtGui> 
#include "segInspector.h" 

#include "common/omStd.h"
#include "volume/omVolume.h"

#include <QThread>
#include <qtconcurrentrun.h>

#define DEBUG 0

SegInspector::SegInspector(OmId seg_id, QWidget *parent) 
: QWidget(parent) 
{ 
    setupUi(this);
	
	my_id = seg_id;
	directoryEdit->setReadOnly(true);
}

void SegInspector::setId(OmId new_id)
{
	my_id = new_id;
}

void SegInspector::on_nameEdit_editingFinished()
{
	OmVolume::GetSegmentation(my_id).SetName(nameEdit->text().toStdString());
}

void SegInspector::on_browseButton_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
													"", QFileDialog::ShowDirsOnly);
	if(dir != "") {
		if(! dir.endsWith("/")) 
			dir += QString("/");
		directoryEdit->setText(dir);
		
		OmVolume::GetSegmentation(my_id).SetSourceDirectoryPath(dir.toStdString());
		
		listWidget->clear();
		list<string>::const_iterator match_it;
		const list<string> &matches = OmVolume::GetSegmentation(my_id).GetSourceFilenameRegexMatches();

		for(match_it = matches.begin(); match_it != matches.end(); ++match_it) {
			listWidget->addItem(QString::fromStdString(*match_it));
		}
	}
}

void SegInspector::on_exportButton_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Export As"));
	if(fileName == NULL)
		return;
	
	QString fname = fileName.section('/', -1);
	QString dpath = fileName.remove(fname);
	
	OmVolume::GetSegmentation(my_id).ExportInternalData(dpath.toStdString(), fname.toStdString());
}

void SegInspector::on_patternEdit_editingFinished()
{
	OmVolume::GetSegmentation(my_id).SetSourceFilenameRegex(patternEdit->text().toStdString());
}

void SegInspector::on_patternEdit_textChanged()
{
	
	OmVolume::GetSegmentation(my_id).SetSourceFilenameRegex(patternEdit->text().toStdString());
	
	// change list view - listWidget
	//GetSourceFileRegexMatches(list<string> &)
	listWidget->clear();
	list<string>::const_iterator match_it;
	const list<string> &matches = OmVolume::GetSegmentation(my_id).GetSourceFilenameRegexMatches();
	
	for(match_it = matches.begin(); match_it != matches.end(); ++match_it) {
		listWidget->addItem(QString::fromStdString(*match_it));
	}
}



void build_image(OmSegmentation *current_seg) {
	DOUT("build image");
	current_seg->BuildVolumeData();
}

void build_mesh(OmSegmentation *current_seg) {
	DOUT("build mesh");
	current_seg->BuildMeshData();
}

void SegInspector::on_buildButton_clicked()
{
	DOUT("SegInspector::on_buildButton_clicked");
	
	// check current selection in buildComboBox
	QString cur_text = buildComboBox->currentText();
	
	extern void build_image(OmSegmentation *current_seg);
	extern void build_mesh(OmSegmentation *current_seg);
	
	OmSegmentation &current_seg = OmVolume::GetSegmentation(my_id);
	
	if(cur_text == QString("Data")) {
		//OmVolume::GetSegmentation(my_id).BuildVolumeData();
		QFuture<void> future = QtConcurrent::run(build_image, &current_seg);
		emit segmentationBuilt( my_id );
	}
	else if(cur_text == QString("Mesh")) {
		//	OmVolume::GetSegmentation(my_id).BuildMeshData();
		
		QFuture<void> future = QtConcurrent::run(build_mesh, &current_seg);
		//		emit meshBuilt(my_id);
	}
	else if(cur_text == QString("Data & Mesh"))
	{
		QFuture<void> f1 = QtConcurrent::run(build_image, &current_seg);
		f1.waitForFinished();
		QFuture<void> f2 = QtConcurrent::run(build_mesh, &current_seg);
		//		emit meshBuilt(my_id);
		emit segmentationBuilt( my_id );

	}
}

void SegInspector::on_notesEdit_textChanged()
{
	OmVolume::GetSegmentation(my_id).SetNote(notesEdit->toPlainText().toStdString());
}


void SegInspector::setSegmentationID( const OmId segmenID )
{
	SegmentationID = segmenID;
}

OmId SegInspector::getSegmentationID()
{
	return SegmentationID;
}

void SegInspector::setSegmentID( const OmId segID )
{
	SegmentID = segID;
}

OmId SegInspector::getSegmentID()
{
	return SegmentID;
}
