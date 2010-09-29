#include "gui/meshPreviewer/meshPreviewer.hpp"
#include "utility/dataWrappers.h"
#include "gui/inspectors/segmentation/exportSegmentList.hpp"
#include "gui/inspectors/segmentation/exportMST.hpp"
#include "gui/inspectors/segmentation/segInspector.h"
#include "gui/inspectors/segmentation/addSegmentButton.h"
#include "gui/inspectors/segmentation/buildButton.hpp"
#include "gui/inspectors/segmentation/exportButton.hpp"
#include "gui/inspectors/segmentation/meshPreviewButton.hpp"
#include "gui/inspectors/volInspector.h"
#include "gui/myInspectorWidget.h"
#include "system/omLocalPreferences.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "utility/sortHelpers.h"
#include "utility/stringHelpers.h"

SegInspector::SegInspector( const SegmentationDataWrapper incoming_sdw,
							MyInspectorWidget* parent)
	: QWidget(parent)
	, inspectorWidget_(parent)
{
	sdw = boost::make_shared<SegmentationDataWrapper>(incoming_sdw);

	QVBoxLayout* overallContainer = new QVBoxLayout(this);
	overallContainer->addWidget(makeSourcesBox());
	overallContainer->addWidget(makeActionsBox());
	overallContainer->addWidget(makeToolsBox());
	overallContainer->addWidget(makeStatsBox());
	overallContainer->addWidget(makeVolBox());
	overallContainer->addWidget(makeNotesBox());

	populateSegmentationInspector();
	directoryEdit->setReadOnly(true);

	QMetaObject::connectSlotsByName(this);
}

QGroupBox* SegInspector::makeVolBox()
{
	return new OmVolInspector(&sdw->getSegmentation(), this);
}

QGroupBox* SegInspector::makeStatsBox()
{
	QGroupBox* statsBox = new QGroupBox("Stats");
	QGridLayout *grid = new QGridLayout( statsBox );

	QLabel *labelNumSegments = new QLabel(statsBox);
	labelNumSegments->setText("number of segments:");
	grid->addWidget( labelNumSegments, 0, 0 );
	QLabel *labelNumSegmentsNum = new QLabel(statsBox);

	QString commaNumSegs = StringHelpers::commaDeliminateNumQT(sdw->getNumberOfSegments());
	labelNumSegmentsNum->setText( commaNumSegs );
	grid->addWidget( labelNumSegmentsNum, 0, 1 );


	QLabel *labelNumTopSegments = new QLabel(statsBox);
	labelNumTopSegments->setText("number of top-level segments:");
	grid->addWidget( labelNumTopSegments, 1, 0 );
	QLabel *labelNumTopSegmentsNum = new QLabel(statsBox);

	QString commaNumTopSegs = StringHelpers::commaDeliminateNumQT(sdw->getNumberOfTopSegments());
	labelNumTopSegmentsNum->setText( commaNumTopSegs );
	grid->addWidget( labelNumTopSegmentsNum, 1, 1 );

	return statsBox;
}

QGroupBox* SegInspector::makeNotesBox()
{
	QGroupBox* notesBox = new QGroupBox("Notes");
	QGridLayout *gridNotes = new QGridLayout( notesBox );

	notesEdit = new QPlainTextEdit(notesBox);
	notesEdit->setObjectName(QString::fromUtf8("notesEdit"));
	gridNotes->addWidget(notesEdit, 0, 1);

	return notesBox;
}

QGroupBox* SegInspector::makeToolsBox()
{
	QGroupBox* segmentBox = new QGroupBox("Tools");
	QGridLayout *gridSegment = new QGridLayout( segmentBox );

	addSegmentButton = new AddSegmentButton(this);
	gridSegment->addWidget(addSegmentButton, 0, 0);

	return segmentBox;
}

QGroupBox* SegInspector::makeActionsBox()
{
	QGroupBox* actionsBox = new QGroupBox("Actions");
	QGridLayout *gridAction = new QGridLayout( actionsBox );

	buildComboBox = new QComboBox(actionsBox);
	buildComboBox->setObjectName(QString::fromUtf8("buildComboBox"));
	buildComboBox->clear();
	buildComboBox->insertItems(0, QStringList()
							   << "Data"
							   << "Mesh"
							   << "Data & Mesh"
							   << "Load Dendrogram"
							   << "Blank Volume"
        );
	gridAction->addWidget(buildComboBox, 1, 0);

	BuildButton* buildButton = new BuildButton(this);
	gridAction->addWidget(buildButton, 1, 1);

	ExportButton* exportButton = new ExportButton(this);
	gridAction->addWidget(exportButton, 2, 0, 1, 2 );

	ExportSegmentList* segList = new ExportSegmentList(this);
	gridAction->addWidget(segList, 3, 0, 1, 2 );

	ExportMST* mstList = new ExportMST(this);
	gridAction->addWidget(mstList, 4, 0, 1, 2 );

	MeshPreviewButton* meshPreviewButton = new MeshPreviewButton(this);
	gridAction->addWidget(meshPreviewButton, 5, 0, 1, 2 );

	return actionsBox;
}

QGroupBox* SegInspector::makeSourcesBox()
{
	QGroupBox* sourceBox = new QGroupBox("Source");
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
	patternLabel->setToolTip("(i.e. data.%d.tif)");
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

	return sourceBox;
}

void SegInspector::on_nameEdit_editingFinished()
{
	sdw->getSegmentation().SetCustomName(nameEdit->text());
}

void SegInspector::on_browseButton_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
													"", QFileDialog::ShowDirsOnly);
	if (dir != "") {
		directoryEdit->setText(dir);
		updateFileList();
	}
}

QDir SegInspector::getDir()
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

QStringList SegInspector::getFileList()
{
	QStringList files = getDir().entryList();
	return SortHelpers::SortNaturally( files );
}

QFileInfoList SegInspector::getFileInfoList()
{
	QFileInfoList files = getDir().entryInfoList();
	return SortHelpers::SortNaturally( files );
}

void SegInspector::updateFileList()
{
	listWidget->clear();

	foreach (QString fn, getFileList() ) {
		listWidget->addItem( fn );
	}

	listWidget->update();
}

void SegInspector::on_patternEdit_textChanged()
{
	updateFileList();
}

void SegInspector::on_notesEdit_textChanged()
{
	OmProject::GetSegmentation(sdw->getID()).SetNote(notesEdit->toPlainText());
}

OmId SegInspector::getSegmentationID()
{
	return sdw->getID();
}

void SegInspector::populateSegmentationInspector()
{
	nameEdit->setText( sdw->getName() );
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

	notesEdit->setPlainText( sdw->getNote() );

	updateFileList();
}

void SegInspector::rebuildSegmentLists(const OmId segmentationID, const OmSegID segID)
{
	inspectorWidget_->rebuildSegmentLists(segmentationID, segID);
}

boost::shared_ptr<SegmentationDataWrapper>
SegInspector::getSegmentationDataWrapper(){
	return sdw;
}

OmViewGroupState* SegInspector::getViewGroupState(){
	return inspectorWidget_->getViewGroupState();
}
