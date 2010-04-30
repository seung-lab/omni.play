#include "volumeInspector.h"

#include "volume/omVolume.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include "common/omDebug.h"

VolumeInspector::VolumeInspector(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	QGroupBox* groupBox = new QGroupBox("Volume");
	overallContainer->addWidget( groupBox );

	QGridLayout *grid = new QGridLayout( groupBox );

	grid->addWidget( makeSrcPropBox(), 0, 0 );
	grid->addWidget( makeVolPropBox(), 1, 0 );
	grid->addWidget( makeNotesBox(), 4, 0 );
	grid->setRowStretch( 5, 1 );

	init_values();
        QMetaObject::connectSlotsByName(this);
}

QGroupBox* VolumeInspector::makeNotesBox()
{
	QGroupBox* groupBox = new QGroupBox("Notes");
	QVBoxLayout* vbox = new QVBoxLayout();
	groupBox->setLayout( vbox );

        notesEdit = new QPlainTextEdit(groupBox);
        notesEdit->setObjectName(QString("notesEdit"));
	vbox->addWidget(notesEdit);
 
	return groupBox;
}

QGroupBox* VolumeInspector::makeVolPropBox()
{
	QGroupBox* groupBox = new QGroupBox("Volume Properties");
	QGridLayout* gridLayout = new QGridLayout;
	groupBox->setLayout( gridLayout );

	QLabel* dimLabel = new QLabel(groupBox);
        dimLabel->setObjectName(QString("dimLabel"));
	dimLabel->setText("Size:");
	gridLayout->addWidget(dimLabel, 0, 0, 1, 1);

	dimSizeLabel = new QLabel(groupBox);
        dimSizeLabel->setObjectName(QString("dimSizeLabel"));
	SizeLabelUpdate();
	gridLayout->addWidget(dimSizeLabel, 0, 1, 1, 1);

        QLabel* resolutionLabel = new QLabel(groupBox);
        resolutionLabel->setObjectName(QString("resolutionLabel"));
	resolutionLabel->setToolTip("X, Y, Z");
        resolutionLabel->setText("Pixel Resolution:");
        gridLayout->addWidget(resolutionLabel, 1, 0, 1, 1);

        resolutionEdit = new QLineEdit(groupBox);
        resolutionEdit->setObjectName(QString("resolutionEdit"));
	resolutionEdit->setToolTip("X,Y,Z");
        gridLayout->addWidget(resolutionEdit, 1, 1, 1, 1);

        QLabel* unitLabel = new QLabel(groupBox);
        unitLabel->setObjectName(QString("unitLabel"));
	unitLabel->setToolTip("X, Y, Z");
        unitLabel->setText("Resolution Units:");
        gridLayout->addWidget(unitLabel, 2, 0, 1, 1);

        unitList = new QComboBox(groupBox);
        unitList->setObjectName(QString("unitList"));
	unitList->setToolTip("X,Y,Z");
	QChar unit[3];
	// add angstrom symbol
	unit[0]=8491;
	unitList->addItem(QString(unit,1));
	unitList->addItem(QString("nm"));
	// add greek letter 'mu' and letter 'm'
	unit[0]=956;
	unit[1]=109;
	unitList->addItem(QString(unit,2));
        gridLayout->addWidget(unitList, 2, 1, 1, 1);

        QLabel* extentLabel = new QLabel(groupBox);
        extentLabel->setObjectName(QString("extentLabel"));
        extentLabel->setToolTip("(in pixels)");
        extentLabel->setText("Data Extent:");
        gridLayout->addWidget(extentLabel, 3, 0, 1, 1);

        extentEdit = new QLineEdit(groupBox);
        extentEdit->setObjectName(QString("extentEdit"));
        gridLayout->addWidget(extentEdit, 3, 1, 1, 1);


	QLabel* lengthLabel = new QLabel(groupBox);
        lengthLabel->setObjectName(QString("lengthLabel"));
        lengthLabel->setToolTip("(in pixels)");
        lengthLabel->setText("Chunk Size:");
        gridLayout->addWidget(lengthLabel, 4, 0, 1, 1);

        sizeSlider = new QSlider(groupBox);
        sizeSlider->setObjectName(QString("sizeSlider"));
        sizeSlider->setMinimum(8);
        sizeSlider->setMaximum(256);
        sizeSlider->setSingleStep(1);
        sizeSlider->setOrientation(Qt::Horizontal);
        sizeSlider->setTickPosition(QSlider::TicksBelow);
        gridLayout->addWidget(sizeSlider, 4, 1, 1, 1);

        sizeLabel = new QLabel(groupBox);
        sizeLabel->setObjectName(QString("sizeLabel"));
	sizeLabel->setText("size");
	gridLayout->addWidget(sizeLabel, 5, 1, 1, 1);

	return groupBox;
}

QGroupBox* VolumeInspector::makeSrcPropBox()
{
	QGroupBox* groupBox = new QGroupBox("Source Properties");
	QGridLayout* gridLayout = new QGridLayout;
	groupBox->setLayout( gridLayout );

	QLabel* nameLabel = new QLabel(groupBox);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));
        nameLabel->setText("Name:");
        gridLayout->addWidget(nameLabel, 0, 0 );

        nameEdit = new QLineEdit(groupBox);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));
	nameEdit->setText(QString());
	gridLayout->addWidget(nameEdit, 0, 1);

	return groupBox;
}

void VolumeInspector::init_values()
{
	nameEdit->setText( (*OmVolume::Instance()).GetName() );
	nameEdit->setMinimumWidth(200);

	notesEdit->setPlainText( (*OmVolume::Instance()).GetNote() );

	Vector3 < float >res = OmVolume::GetDataResolution();
	resolutionEdit->setText( QString("[%1 %2 %3]")
				 .arg(res.x)
				 .arg(res.y)
				 .arg(res.z) );

	Vector3i dimen = OmVolume::GetDataDimensions();
	extentEdit->setText( QString("[%1 %2 %3]")
			     .arg(dimen.x)
			     .arg(dimen.y)
			     .arg(dimen.z) );

	int my_chunk_size = OmVolume::GetChunkDimension();
	sizeSlider->setSliderPosition(my_chunk_size / 2);
	sizeLabel->setNum(my_chunk_size);
}

void VolumeInspector::on_nameEdit_editingFinished()
{
	OmVolume::Instance()->SetName(nameEdit->text());
}

void VolumeInspector::on_scaleEdit_editingFinished()
{
	QString scale = scaleEdit->text();
	scale.chop(1);
	scale = scale.remove(0, 1);

	QString x = scale.section(" ", 0, 0);
	QString y = scale.section(" ", 1, 1);
	QString z = scale.section(" ", 2, 2);

	Vector3 < float >scale_vec = Vector3 < float >(x.toFloat(), y.toFloat(), z.toFloat());
	OmVolume::Instance()->SetUserScale(scale_vec);
}

void VolumeInspector::on_resolutionEdit_editingFinished()
{
	QString res = resolutionEdit->text();
	res.chop(1);
	res = res.remove(0, 1);

	QString x = res.section(" ", 0, 0);
	QString y = res.section(" ", 1, 1);
	QString z = res.section(" ", 2, 2);

	Vector3 < float >res_vec = Vector3 < float >(x.toFloat(), y.toFloat(), z.toFloat());
	OmVolume::Instance()->SetDataResolution(res_vec);
	SizeLabelUpdate();
}

void VolumeInspector::on_extentEdit_editingFinished()
{
	QString extent = extentEdit->text();
	extent.chop(1);
	extent = extent.remove(0, 1);

	QString min_x = extent.section(" ", 0, 0);
	QString min_y = extent.section(" ", 1, 1);
	QString min_z = extent.section(" ", 2, 2);

	Vector3i data_extent = Vector3i(min_x.toInt(), min_y.toInt(), min_z.toInt());

	OmVolume::Instance()->SetDataDimensions(data_extent);
}

void VolumeInspector::on_unitList_activated()
{
	OmVolume::SetUnit(unitList->currentText());
	SizeLabelUpdate();
}

void VolumeInspector::on_notesEdit_textChanged()
{
	OmVolume::Instance()->SetNote(notesEdit->toPlainText());
}

void VolumeInspector::on_sizeSlider_valueChanged()
{
	sizeLabel->setNum(sizeSlider->value() * 2);
	OmVolume::SetChunkDimension(sizeSlider->value() * 2);
}

void VolumeInspector::SizeLabelUpdate()
{
	Vector3i dataExtent = OmVolume::GetDataDimensions();
	Vector3f dataResolution = OmVolume::GetDataResolution();
	Vector3f spaceExtent = OmVolume::GetScale();
	QString unit = OmVolume::GetUnit();

	QString messageString = QString("%1 x %2 x %3 %4")
		.arg(spaceExtent.x)
		.arg(spaceExtent.y)
		.arg(spaceExtent.z)
		.arg(unit);

	dimSizeLabel->setText(messageString);
	return;
}
