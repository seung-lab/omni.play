#include "segInspector.h"
#include "volInspector.h"

#include "common/omDebug.h"
#include "volume/omVolume.h"
#include "project/omProject.h"
#include "system/omLocalPreferences.h"
#include "system/omProjectData.h"
#include "system/omBuildSegmentation.h"
#include "utility/sortHelpers.h"
#include "utility/stringHelpers.h"
#include "system/omStateManager.h"

SegInspector::SegInspector( const SegmentationDataWrapper incoming_sdw, QWidget * parent)
 : QWidget(parent)
{
	sdw = incoming_sdw;

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

	mMeshinatorProc = NULL;
	mMeshinatorDialog = NULL;
}

QGroupBox* SegInspector::makeVolBox()
{
        OmSegmentation & seg = OmProject::GetSegmentation(sdw.getID());
	return new OmVolInspector(&seg, this);
}

QGroupBox* SegInspector::makeStatsBox()
{
	QGroupBox* statsBox = new QGroupBox("Stats");
	QGridLayout *grid = new QGridLayout( statsBox );

	QLabel *labelNumSegments = new QLabel(statsBox);
	labelNumSegments->setText("number of segments:");
	grid->addWidget( labelNumSegments, 0, 0 );
	QLabel *labelNumSegmentsNum = new QLabel(statsBox);

	QString commaNumSegs = StringHelpers::commaDeliminateNumber( sdw.getNumberOfSegments() );
	labelNumSegmentsNum->setText( commaNumSegs );
	grid->addWidget( labelNumSegmentsNum, 0, 1 );


	QLabel *labelNumTopSegments = new QLabel(statsBox);
	labelNumTopSegments->setText("number of top-level segments:");
	grid->addWidget( labelNumTopSegments, 1, 0 );
	QLabel *labelNumTopSegmentsNum = new QLabel(statsBox);

	QString commaNumTopSegs = StringHelpers::commaDeliminateNumber( sdw.getNumberOfTopSegments() );
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

	addSegmentButton = new QPushButton(segmentBox);
        addSegmentButton->setObjectName(QString::fromUtf8("addSegmentButton"));
        addSegmentButton->setText("Add Segment");
        gridSegment->addWidget(addSegmentButton, 0, 0);

	return segmentBox;
}

QGroupBox* SegInspector::makeActionsBox()
{
	QGroupBox* actionsBox = new QGroupBox("Actions");
	QGridLayout *gridAction = new QGridLayout( actionsBox );

	QLabel *labelNumThreadsText = new QLabel(actionsBox);
	labelNumThreadsText->setText("number of meshing threads: ");
	gridAction->addWidget( labelNumThreadsText, 0, 0 );
	QLabel *labelNumThreadsNum = new QLabel(actionsBox);
	labelNumThreadsNum->setNum( OmLocalPreferences::numAllowedWorkerThreads());
	gridAction->addWidget( labelNumThreadsNum, 0, 1 );

	buildComboBox = new QComboBox(actionsBox);
        buildComboBox->setObjectName(QString::fromUtf8("buildComboBox"));
        buildComboBox->clear();
        buildComboBox->insertItems(0, QStringList()
         << "Data"
         << "Mesh"
	 << "Data & Mesh"
         << "Load Dendrogram"
         << "Meshinator"
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
        gridAction->addWidget(exportButton, 2, 0, 1, 2 );

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
	OmProject::GetSegmentation(sdw.getID()).SetName(nameEdit->text());
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

void SegInspector::on_exportButton_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Export As"));
	if (fileName == NULL)
		return;

	OmProject::GetSegmentation(sdw.getID()).ExportInternalData(fileName);
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
	return SortHelpers::sortNaturally( files );
}

QFileInfoList SegInspector::getFileInfoList()
{
	QFileInfoList files = getDir().entryInfoList();
	return SortHelpers::sortNaturally( files );
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

QString& GetScriptCmd (QString arg)
{
	static QString cmd;

	QString omniPath = OmStateManager::getOmniExecutableAbsolutePath();
	debug ("meshinator", "%s\n", qPrintable (omniPath));
	QString cmdPath = omniPath; 

	cmdPath.truncate (omniPath.size () - 13);  // "omni/bin/omni" == 13
	cmd = cmdPath;
	cmd += "/scripts/cluster/headnodemesher.pl " + arg;

	return cmd;
}

void SegInspector::on_buildButton_clicked()
{
	OmSegmentation & current_seg = OmProject::GetSegmentation(sdw.getID());

	OmBuildSegmentation * bs = new OmBuildSegmentation(&current_seg);
	bs->setFileNamesAndPaths( getFileInfoList() );

	QString whatOrHowToBuild = buildComboBox->currentText();
	if ("Data" == whatOrHowToBuild ){
		bs->build_seg_image();
		emit segmentationBuilt(sdw.getID());

	} else if ( "Mesh" == whatOrHowToBuild ){
		bs->build_seg_mesh();

	} else if ("Data & Mesh" == whatOrHowToBuild){
		bs->buildAndMeshSegmentation();
		emit segmentationBuilt(sdw.getID());

	} else if ("Load Dendrogram" == whatOrHowToBuild){
		bs->loadDendrogram();
		emit segmentationBuilt(sdw.getID());

	} else if( "Meshinator" == whatOrHowToBuild ){
		doMeshinate( &current_seg );
	}
}

void SegInspector::doMeshinate( OmSegmentation * current_seg )
{
	QString abs_fnpn = OmProjectData::getAbsoluteFileNameAndPath();
	QString fnpnPlan = abs_fnpn + ".plan";
	current_seg->BuildMeshDataPlan( fnpnPlan );

	QString script = GetScriptCmd (fnpnPlan);
	debug ("meshinator", "%s\n", qPrintable (script));
	if (mMeshinatorProc) {
		delete mMeshinatorProc;
	}
		
	mMeshinatorProc = new QProcess ();
	mMeshinatorProc->start(script);

	if (mMeshinatorDialog) {
		delete mMeshinatorDialog;
	}
	mMeshinatorDialog = new QDialog ();
	connect(mMeshinatorProc, SIGNAL(finished(int)), mMeshinatorDialog, SLOT(close()) );
	mMeshinatorDialog->exec();

	OmProject::Save();
}

void SegInspector::on_notesEdit_textChanged()
{
	OmProject::GetSegmentation(sdw.getID()).SetNote(notesEdit->toPlainText());
}

OmId SegInspector::getSegmentationID()
{
	return sdw.getID();
}

void SegInspector::populateSegmentationInspector()
{
	nameEdit->setText( sdw.getName() );
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

	notesEdit->setPlainText( sdw.getNote() );

	updateFileList();
}
