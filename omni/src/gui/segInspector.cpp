#include "segInspector.h"

#include "common/omStd.h"
#include "volume/omVolume.h"

#include <QThread>
#include <qtconcurrentrun.h>
#include "system/omDebug.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"


#define DEBUG 0

SegInspector::SegInspector(OmId seg_id, QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout(this);

	// Source group box
	QGroupBox* sourceBox = new QGroupBox("Source");
	overallContainer->addWidget( sourceBox );

	QGridLayout *grid = new QGridLayout( sourceBox );

	QLabel *nameLabel = new QLabel(sourceBox);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));
        nameLabel->setText("Name:");
        grid->addWidget(nameLabel, 0, 0);

	nameEdit = new QLineEdit(sourceBox);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));
        nameEdit->setText(QString());
        grid->addWidget(nameEdit, 0, 1);

	directoryLabel = new QLabel(sourceBox);
        directoryLabel->setObjectName(QString::fromUtf8("directoryLabel"));
        directoryLabel->setText("Directory:");
        grid->addWidget(directoryLabel, 1, 0);

	directoryEdit = new QLineEdit(sourceBox);
        directoryEdit->setObjectName(QString::fromUtf8("directoryEdit"));

        grid->addWidget(directoryEdit, 1, 1);

	QPushButton *browseButton = new QPushButton(sourceBox);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));
        browseButton->setText("Browse");
        grid->addWidget(browseButton, 2, 1);

	QLabel *patternLabel = new QLabel(sourceBox);
        patternLabel->setObjectName(QString::fromUtf8("patternLabel"));
        patternLabel->setText("Pattern:");
#ifndef QT_NO_TOOLTIP
        patternLabel->setToolTip("(i.e. data.%d.tif)");
#endif // QT_NO_TOOLTIP
        grid->addWidget(patternLabel, 3, 0);

	patternEdit = new QLineEdit(sourceBox);
        patternEdit->setObjectName(QString::fromUtf8("patternEdit"));
        grid->addWidget(patternEdit, 3, 1);

        listWidget = new QListWidget(sourceBox);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(listWidget->sizePolicy().hasHeightForWidth());
        listWidget->setSizePolicy(sizePolicy1);
        listWidget->setMaximumSize(QSize(16777215, 85));
        grid->addWidget(listWidget, 4, 1 );


	// Actions group box
	QGroupBox* actionsBox = new QGroupBox("Actions");
	overallContainer->addWidget( actionsBox );

	QGridLayout *gridAction = new QGridLayout( actionsBox );

	QLabel *labelNumThreadsText = new QLabel(actionsBox);
	labelNumThreadsText->setText("number of meshing threads: ");
	gridAction->addWidget( labelNumThreadsText, 0, 0 );
	QLabel *labelNumThreadsNum = new QLabel(actionsBox);
	labelNumThreadsNum->setNum( OmPreferences::GetInteger(OM_PREF_MESH_NUM_MESHING_THREADS_INT ));
	gridAction->addWidget( labelNumThreadsNum, 0, 1 );

	buildComboBox = new QComboBox(actionsBox);
        buildComboBox->setObjectName(QString::fromUtf8("buildComboBox"));
        buildComboBox->clear();
        buildComboBox->insertItems(0, QStringList()
         << "Data"
         << "Mesh"
         << "Data & Mesh"
        );
        gridAction->addWidget(buildComboBox, 1, 0);

	QPushButton *buildButton = new QPushButton(actionsBox);
        buildButton->setObjectName(QString::fromUtf8("buildButton"));
        buildButton->setEnabled(true);
        buildButton->setText("Build");
        gridAction->addWidget(buildButton, 1, 1);

	QPushButton *exportButton = new QPushButton(actionsBox);
        exportButton->setObjectName(QString::fromUtf8("exportButton"));
        exportButton->setText("Export");
        gridAction->addWidget(exportButton, 2, 0);


	// Tools group box
	QGroupBox* segmentBox = new QGroupBox("Tools");
	overallContainer->addWidget( segmentBox );

	QGridLayout *gridSegment = new QGridLayout( segmentBox );

	addSegmentButton = new QPushButton(segmentBox);
        addSegmentButton->setObjectName(QString::fromUtf8("addSegmentButton"));
        addSegmentButton->setText("Add Segment");
        gridSegment->addWidget(addSegmentButton, 0, 0);


	// Notes group box
	QGroupBox* notesBox = new QGroupBox("Notes");
	overallContainer->addWidget( notesBox );

	QGridLayout *gridNotes = new QGridLayout( notesBox );

	notesEdit = new QPlainTextEdit(notesBox);
        notesEdit->setObjectName(QString::fromUtf8("notesEdit"));
        gridNotes->addWidget(notesEdit, 0, 1);

     
        QMetaObject::connectSlotsByName(this);

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
	if (dir != "") {
		if (!dir.endsWith("/"))
			dir += QString("/");
		directoryEdit->setText(dir);

		OmVolume::GetSegmentation(my_id).SetSourceDirectoryPath(dir.toStdString());

		listWidget->clear();
		list < string >::const_iterator match_it;
		const list < string > &matches = OmVolume::GetSegmentation(my_id).GetSourceFilenameRegexMatches();

		for (match_it = matches.begin(); match_it != matches.end(); ++match_it) {
			listWidget->addItem(QString::fromStdString(*match_it));
		}
	}
}

void SegInspector::on_exportButton_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Export As"));
	if (fileName == NULL)
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

	listWidget->clear();
	list < string >::const_iterator match_it;
	const list < string > &matches = OmVolume::GetSegmentation(my_id).GetSourceFilenameRegexMatches();

	for (match_it = matches.begin(); match_it != matches.end(); ++match_it) {
		listWidget->addItem(QString::fromStdString(*match_it));
	}

	listWidget->update();
}

void build_image(OmSegmentation * current_seg)
{
	//debug("genone","build image");
	current_seg->BuildVolumeData();
}

void build_mesh(OmSegmentation * current_seg)
{
	//debug("genone","build mesh");
	current_seg->BuildMeshData();
}

void SegInspector::on_buildButton_clicked()
{
	//debug("genone","SegInspector::on_buildButton_clicked");

	// check current selection in buildComboBox
	QString cur_text = buildComboBox->currentText();

	extern void build_image(OmSegmentation * current_seg);
	extern void build_mesh(OmSegmentation * current_seg);

	OmSegmentation & current_seg = OmVolume::GetSegmentation(my_id);

	if (cur_text == QString("Data")) {
		//OmVolume::GetSegmentation(my_id).BuildVolumeData();
		QFuture < void >future = QtConcurrent::run(build_image, &current_seg);
		emit segmentationBuilt(my_id);
	} else if (cur_text == QString("Mesh")) {
		//      OmVolume::GetSegmentation(my_id).BuildMeshData();

		QFuture < void >future = QtConcurrent::run(build_mesh, &current_seg);
		//              emit meshBuilt(my_id);
	} else if (cur_text == QString("Data & Mesh")) {
		QFuture < void >f1 = QtConcurrent::run(build_image, &current_seg);
		f1.waitForFinished();
		QFuture < void >f2 = QtConcurrent::run(build_mesh, &current_seg);
		//              emit meshBuilt(my_id);
		emit segmentationBuilt(my_id);

	}
}

void SegInspector::on_notesEdit_textChanged()
{
	OmVolume::GetSegmentation(my_id).SetNote(notesEdit->toPlainText().toStdString());
}

void SegInspector::setSegmentationID(const OmId segmenID)
{
	SegmentationID = segmenID;
}

OmId SegInspector::getSegmentationID()
{
	return SegmentationID;
}

void SegInspector::setSegmentID(const OmId segID)
{
	SegmentID = segID;
}

OmId SegInspector::getSegmentID()
{
	return SegmentID;
}
