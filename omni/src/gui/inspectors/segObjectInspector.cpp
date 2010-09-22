#include "utility/dataWrappers.h"
#include "segObjectInspector.h"
#include "common/omDebug.h"
#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omViewEvent.h"
#include "system/cache/omCacheManager.h"
#include "utility/stringHelpers.h"

SegObjectInspector::SegObjectInspector(SegmentDataWrapper sdw_, QWidget* parent)
 : QWidget(parent)
{
	sdw = boost::make_shared<SegmentDataWrapper>(sdw_);

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

	nameEdit->setText( sdw->getName() );
	nameEdit->setMinimumWidth(200);

	segmentIDEdit->setText( sdw->getIDstr() );
	segmentIDEdit->setMinimumWidth(200);

	notesEdit->setPlainText( sdw->getNote() );

	const Vector3 < float >&color = sdw->getColorFloat();

	QPixmap *pixm = new QPixmap(40, 30);
	QColor newcolor = qRgb(color.x * 255, color.y * 255, color.z * 255);
	pixm->fill(newcolor);

	colorButton->setIcon(QIcon(*pixm));
	current_color = newcolor;

	sizeNoChildren->setText( StringHelpers::commaDeliminateNumber(sdw->getSize()));
	sizeWithChildren->setText( StringHelpers::commaDeliminateNumber(sdw->getSizeWithChildren()));

	origDataValueList->setText( sdw->getIDstr() );
	chunkList->setText( "disabled" );
}

void SegObjectInspector::nameEditChanged()
{
	sdw->setName( nameEdit->text() );
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

	Vector3 < float >color_vector(color.redF()/2, color.greenF()/2, color.blueF()/2);
	sdw->setColor(color_vector);

	OmCacheManager::TouchFresheness();
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

	QLabel* sizeLabelNoChildren = new QLabel(sourceBox);
	sizeLabelNoChildren->setObjectName(QString::fromUtf8("sizeLabelNoChildren"));
	sizeLabelNoChildren->setText( "Size (voxels--no children):" );
        grid->addWidget(sizeLabelNoChildren, 5, 0);

	sizeNoChildren = new QLabel(sourceBox);
	sizeNoChildren->setObjectName(QString::fromUtf8("sizeNoChildren"));
        grid->addWidget(sizeNoChildren, 5, 1);

	QLabel* sizeLabelWithChildren = new QLabel(sourceBox);
	sizeLabelWithChildren->setObjectName(QString::fromUtf8("sizeLabelWithChildren"));
	sizeLabelWithChildren->setText( "Size (voxels--with children):" );
        grid->addWidget(sizeLabelWithChildren, 6, 0);

	sizeWithChildren = new QLabel(sourceBox);
	sizeWithChildren->setObjectName(QString::fromUtf8("sizeWithChildren"));
        grid->addWidget(sizeWithChildren, 6, 1);

	QLabel* chunkLabel = new QLabel(sourceBox);
	chunkLabel->setObjectName(QString::fromUtf8("chunkLabel"));
	chunkLabel->setText( "Chunks" );
        grid->addWidget(chunkLabel, 7, 0);

	chunkList = new QLabel(sourceBox);
	chunkList->setObjectName(QString::fromUtf8("chunkList"));
        grid->addWidget(chunkList, 7, 1);

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
