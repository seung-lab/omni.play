#include "segObjectInspector.h"
#include "common/omDebug.h"
#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omViewEvent.h"

SegObjectInspector::SegObjectInspector(SegmentDataWrapper incoming_sdw, QWidget* parent)
 : QWidget(parent)
{
	sdw = incoming_sdw;

	QVBoxLayout* overallContainer = new QVBoxLayout(this);
	overallContainer->addWidget(makeSourcesBox());
	overallContainer->addWidget(makeNotesBox());

	set_initial_values();

	connect(colorButton, SIGNAL(clicked()), 
		this, SLOT(setSegObjColor()), Qt::DirectConnection);

	connect(nameEdit, SIGNAL(editingFinished()),
		this, SLOT(nameEditChanged()), Qt::DirectConnection);
}

void SegObjectInspector::set_initial_values()
{
	segmentIDEdit->setReadOnly(true);

	nameEdit->setText( sdw.getName() );
	nameEdit->setMinimumWidth(200);

	segmentIDEdit->setText( sdw.getIDstr() );
	segmentIDEdit->setMinimumWidth(200);

	notesEdit->setPlainText( sdw.getNote() );

	const Vector3 < float >&color = sdw.getColor();

	QPixmap *pixm = new QPixmap(40, 30);
	QColor newcolor = qRgb(color.x * 255, color.y * 255, color.z * 255);
	pixm->fill(newcolor);

	colorButton->setIcon(QIcon(*pixm));
	current_color = newcolor;

	dataValuesList->setText( sdw.getDataValuesForSegment() );
	origDataValueList->setText( sdw.get_original_mapped_data_value() );
	chunkList->setText( sdw.chunkListStr() );
}

void SegObjectInspector::nameEditChanged()
{
	sdw.setName( nameEdit->text() );
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
}

void SegObjectInspector::setSegObjColor()
{
	QColor color = QColorDialog::getColor(current_color, this);
	if (!color.isValid()) {
		return;
	}

	QPixmap *pixm = new QPixmap(40, 30);
	pixm->fill(color);

	colorButton->setIcon(QIcon(*pixm));
	colorButton->update();
	current_color = color;

	Vector3 < float >color_vector(color.redF(), color.greenF(), color.blueF());
	sdw.setColor(color_vector);

	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}

QGroupBox* SegObjectInspector::makeSourcesBox()
{
	QGroupBox* sourceBox = new QGroupBox("Source Properties");
	QGridLayout *grid = new QGridLayout( sourceBox );

	QLabel *nameLabel = new QLabel(sourceBox);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));
        nameLabel->setText("Name:");
        grid->addWidget(nameLabel, 0, 0);

	nameEdit = new QLineEdit(sourceBox);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));
        nameEdit->setText(QString());
        grid->addWidget(nameEdit, 0, 1);

	QLabel *segmentIDLabel = new QLabel(sourceBox);
        segmentIDLabel->setObjectName(QString::fromUtf8("segmentIDLabel"));
        segmentIDLabel->setText("Segment ID:");
        grid->addWidget(segmentIDLabel, 1, 0);

	segmentIDEdit = new QLineEdit(sourceBox);
        segmentIDEdit->setObjectName(QString::fromUtf8("segmentIDEdit"));
        segmentIDEdit->setText(QString());
        grid->addWidget(segmentIDEdit, 1, 1);

	QLabel* tagsLabel = new QLabel(sourceBox);
        tagsLabel->setObjectName(QString::fromUtf8("tagsLabel"));
        tagsLabel->setText("Tags:");
        grid->addWidget(tagsLabel, 2, 0);

	tagsEdit = new QLineEdit(sourceBox);
        tagsEdit->setObjectName(QString::fromUtf8(""));
        tagsEdit->setText(QString());
        grid->addWidget(tagsEdit, 2, 1);

	QLabel* colorLabel = new QLabel(sourceBox);
	colorLabel->setObjectName(QString::fromUtf8("colorLabel"));
	colorLabel->setText("Color:");
        grid->addWidget(colorLabel, 3, 0);

	colorButton = new QPushButton(sourceBox);
	colorButton->setObjectName(QString::fromUtf8("colorButton"));
	grid->addWidget(colorButton, 3, 1);

	QLabel* origDataValueLabel = new QLabel(sourceBox);
	origDataValueLabel->setObjectName(QString::fromUtf8("origDataValueLabel"));
	origDataValueLabel->setText( "Original Data value:" );
        grid->addWidget(origDataValueLabel, 4, 0);

	origDataValueList = new QLabel(sourceBox);
	origDataValueList->setObjectName(QString::fromUtf8("origDataValueList"));
        grid->addWidget(origDataValueList, 4, 1);

	QLabel* dataValuesLabel = new QLabel(sourceBox);
	dataValuesLabel->setObjectName(QString::fromUtf8("dataValuesLabel"));
	dataValuesLabel->setText( "Data values:" );
        grid->addWidget(dataValuesLabel, 5, 0);

	dataValuesList = new QLabel(sourceBox);
	dataValuesList->setObjectName(QString::fromUtf8("dataValuesList"));
        grid->addWidget(dataValuesList, 5, 1);

	QLabel* chunkLabel = new QLabel(sourceBox);
	chunkLabel->setObjectName(QString::fromUtf8("chunkLabel"));
	chunkLabel->setText( "Chunks" );
        grid->addWidget(chunkLabel, 6, 0);

	chunkList = new QLabel(sourceBox);
	chunkList->setObjectName(QString::fromUtf8("chunkList"));
        grid->addWidget(chunkList, 6, 1);

	return sourceBox;
}

QGroupBox* SegObjectInspector::makeNotesBox()
{
	QGroupBox* notesBox = new QGroupBox("Notes");

	QGridLayout *gridNotes = new QGridLayout( notesBox );

	notesEdit = new QPlainTextEdit(notesBox);
        notesEdit->setObjectName(QString::fromUtf8("notesEdit"));
        gridNotes->addWidget(notesEdit, 0, 1);

	return notesBox;
}
