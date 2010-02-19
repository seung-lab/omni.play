#include "segInspector.h"

#include "common/omStd.h"
#include "volume/omVolume.h"

#include <QThread>
#include <qtconcurrentrun.h>
#include "common/omDebug.h"
#include "system/omProjectData.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include <boost/progress.hpp>

SegInspector::SegInspector( const SegmentationDataWrapper incoming_sdw, QWidget * parent)
 : QWidget(parent)
{
	sdw = incoming_sdw;

	QVBoxLayout* overallContainer = new QVBoxLayout(this);
	overallContainer->addWidget(makeSourcesBox());
	overallContainer->addWidget(makeActionsBox());
	overallContainer->addWidget(makeToolsBox());
	overallContainer->addWidget(makeStatsBox());
	overallContainer->addWidget(makeNotesBox());

	populateSegmentationInspector();
	directoryEdit->setReadOnly(true);

        QMetaObject::connectSlotsByName(this);

	mMeshinatorProc = NULL;
	mMeshinatorDialog = NULL;
}

QGroupBox* SegInspector::makeStatsBox()
{
	QGroupBox* statsBox = new QGroupBox("Stats");
	QGridLayout *grid = new QGridLayout( statsBox );

	QLabel *labelNumSegments = new QLabel(statsBox);
	labelNumSegments->setText("number of segments:");
	grid->addWidget( labelNumSegments, 0, 0 );
	QLabel *labelNumSegmentsNum = new QLabel(statsBox);
	QString numSegs = QString::number( sdw.getNumberOfSegments() );

	QString commaNumSegs;
	QString::iterator i;
	int counter = 0;
	for (i = numSegs.end()-1; i != numSegs.begin()-1; i-- ){
		counter++;
		commaNumSegs.prepend( (*i) );
		if( 0 == ( counter % 3 ) && 
		    counter != numSegs.size() ){
			commaNumSegs.prepend(',');
		}
	}

	labelNumSegmentsNum->setText( commaNumSegs );
	grid->addWidget( labelNumSegmentsNum, 0, 1 );

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
	labelNumThreadsNum->setNum( OmPreferences::GetInteger(OM_PREF_MESH_NUM_MESHING_THREADS_INT ));
	gridAction->addWidget( labelNumThreadsNum, 0, 1 );

	buildComboBox = new QComboBox(actionsBox);
        buildComboBox->setObjectName(QString::fromUtf8("buildComboBox"));
        buildComboBox->clear();
        buildComboBox->insertItems(0, QStringList()
         << "Data"
         << "Mesh"
	 << "Data & Mesh"
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
	OmVolume::GetSegmentation(sdw.getID()).SetName(nameEdit->text().toStdString());
}

void SegInspector::on_browseButton_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
							"", QFileDialog::ShowDirsOnly);
	if (dir != "") {
		if (!dir.endsWith("/"))
			dir += QString("/");
		directoryEdit->setText(dir);

		OmVolume::GetSegmentation(sdw.getID()).SetSourceDirectoryPath(dir.toStdString());

		listWidget->clear();
		list < string >::const_iterator match_it;
		const list < string > &matches = OmVolume::GetSegmentation(sdw.getID()).GetSourceFilenameRegexMatches();

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

	OmVolume::GetSegmentation(sdw.getID()).ExportInternalData(dpath.toStdString(), fname.toStdString());
}

void SegInspector::on_patternEdit_editingFinished()
{
	OmVolume::GetSegmentation(sdw.getID()).SetSourceFilenameRegex(patternEdit->text().toStdString());
}

void SegInspector::on_patternEdit_textChanged()
{
	OmVolume::GetSegmentation(sdw.getID()).SetSourceFilenameRegex(patternEdit->text().toStdString());

	listWidget->clear();
	list < string >::const_iterator match_it;
	const list < string > &matches = OmVolume::GetSegmentation(sdw.getID()).GetSourceFilenameRegexMatches();

	for (match_it = matches.begin(); match_it != matches.end(); ++match_it) {
		listWidget->addItem(QString::fromStdString(*match_it));
	}

	listWidget->update();
}

void build_image(OmSegmentation * current_seg)
{
	printf("starting segmentation data build...\n");
	boost::timer* timer = new boost::timer();
	current_seg->BuildVolumeData();
	const double timeToMeshSecs = timer->elapsed();
	printf("segmentation data build performed in %g (secs?)\n", timeToMeshSecs);
}

void build_mesh(OmSegmentation * current_seg)
{
	printf("starting segmentation mesh build...\n");
	boost::timer* timer = new boost::timer();
	current_seg->BuildMeshData();
	const double timeToMeshSecs = timer->elapsed();
	printf("segmentation mesh build performed in %g (secs?)\n", timeToMeshSecs);
}

void build_image_and_mesh( OmSegmentation * current_seg )
{
	build_image(current_seg);
	build_mesh(current_seg);
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
	// check current selection in buildComboBox
	QString cur_text = buildComboBox->currentText();

	extern void build_image(OmSegmentation * current_seg);
	extern void build_mesh(OmSegmentation * current_seg);
	extern void build_image_and_mesh(OmSegmentation * current_seg);

	OmSegmentation & current_seg = OmVolume::GetSegmentation(sdw.getID());

	if (cur_text == QString("Data")) {
		QFuture < void >future = QtConcurrent::run(build_image, &current_seg);
		emit segmentationBuilt(sdw.getID());
	} else if (cur_text == QString("Mesh")) {
		QFuture < void >future = QtConcurrent::run(build_mesh, &current_seg);
		//              emit meshBuilt(sdw.getID());
	} else if (cur_text == QString("Data & Mesh")) {
		QFuture < void >f1 = QtConcurrent::run(build_image_and_mesh, &current_seg);
		emit segmentationBuilt(sdw.getID());
	} else if( "Meshinator" == cur_text ){
		QString fileName  = QString::fromStdString( OmProjectData::GetFileName() );
		QString pathName  = QString::fromStdString( OmProjectData::GetDirectoryPath() );
		QString rel_fnpn = pathName + fileName;
		QFileInfo fInfo(rel_fnpn);
		QString fnpnProject = fInfo.absoluteFilePath();
		QString fnpnPlan = fnpnProject + ".plan";
		current_seg.BuildMeshDataPlan( fnpnPlan );

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
		mMeshinatorDialog->exec ();
	}
}

void SegInspector::on_notesEdit_textChanged()
{
	OmVolume::GetSegmentation(sdw.getID()).SetNote(notesEdit->toPlainText().toStdString());
}

OmId SegInspector::getSegmentationID()
{
	return sdw.getID();
}

void SegInspector::populateSegmentationInspector()
{
	const OmId s_id = sdw.getID();

	OmSegmentation & current_segmentation = OmVolume::GetSegmentation(s_id);

	const string & my_name = current_segmentation.GetName();
	nameEdit->setText(QString::fromStdString(my_name));
	nameEdit->setMinimumWidth(200);

	const string & my_directory = current_segmentation.GetSourceDirectoryPath();
	directoryEdit->setText(QString::fromStdString(my_directory));
	directoryEdit->setMinimumWidth(200);

	const string & my_pattern = current_segmentation.GetSourceFilenameRegex();
	patternEdit->setText(QString::fromStdString(my_pattern));
	patternEdit->setMinimumWidth(200);

	const string & my_notes = current_segmentation.GetNote();
	notesEdit->setPlainText(QString::fromStdString(my_notes));

	listWidget->clear();

	list < string >::const_iterator match_it;
	const list < string > &matches = OmVolume::GetSegmentation(s_id).GetSourceFilenameRegexMatches();
	for (match_it = matches.begin(); match_it != matches.end(); ++match_it) {
		listWidget->addItem(QString::fromStdString(*match_it));
	}
}
