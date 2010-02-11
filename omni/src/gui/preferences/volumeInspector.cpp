#include "volumeInspector.h"

#include "volume/omVolume.h"
#include "volume/omVolumeTypes.h"
#include "system/omSystemTypes.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include <vmmlib/vmmlib.h>
#include "system/omDebug.h"
using namespace vmml;

VolumeInspector::VolumeInspector(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	QGroupBox* groupBox = new QGroupBox("Volume");
	overallContainer->addWidget( groupBox );

	QGridLayout *grid = new QGridLayout( groupBox );

	grid->addWidget( makeSrcPropBox(), 0, 0 );
	grid->addWidget( makeVolPropBox(), 1, 0 );
	grid->addWidget( makeCachePropBox(), 2, 0 );
	//grid->addWidget( makeAddDataBox(), 3, 0 );
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

/*
QGroupBox* VolumeInspector::makeAddDataBox()
{
	QGroupBox* groupBox = new QGroupBox("Add Data");
	QVBoxLayout* vbox = new QVBoxLayout();
	groupBox->setLayout( vbox );

        addChannelButton = new QPushButton(groupBox);
        addChannelButton->setObjectName(QString("addChannelButton"));
	addChannelButton->setText("Add Channel");
        vbox->addWidget(addChannelButton);

        addSegmentationButton = new QPushButton(groupBox);
        addSegmentationButton->setObjectName(QString("addSegmen"));
        addSegmentationButton->setText("Add Segmentation");
	vbox->addWidget(addSegmentationButton);

	return groupBox;
}
*/
QGroupBox* VolumeInspector::makeCachePropBox()
{
	QGroupBox* groupBox = new QGroupBox("Cache Properties");
	QGridLayout* gridLayout = new QGridLayout;
	groupBox->setLayout( gridLayout );

        QLabel* ramLabel = new QLabel(groupBox);
        ramLabel->setObjectName(QString("ramLabel"));
        ramLabel->setToolTip("(MB)");
        ramLabel->setText("RAM Cache");
        gridLayout->addWidget(ramLabel, 0, 0, 1, 1);

        ramSlider = new QSlider(groupBox);
        ramSlider->setObjectName(QString("ramSlider"));
        ramSlider->setMinimum(100);
        ramSlider->setMaximum(10000);
        ramSlider->setSingleStep(1);
        ramSlider->setOrientation(Qt::Horizontal);
        ramSlider->setTickPosition(QSlider::TicksBelow);
        gridLayout->addWidget(ramSlider, 0, 1, 1, 1);

        ramSizeLabel = new QLabel(groupBox);
        ramSizeLabel->setObjectName(QString("ramSizeLabel"));
	ramSizeLabel->setText("size");
        gridLayout->addWidget(ramSizeLabel, 1, 1, 1, 1);


        vramSizeLabel = new QLabel(groupBox);
        vramSizeLabel->setObjectName(QString("vramSizeLabel"));
        vramSizeLabel->setText("size");
        gridLayout->addWidget(vramSizeLabel, 3, 1, 1, 1);

	vramSlider = new QSlider(groupBox);
        vramSlider->setObjectName(QString("vramSlider"));
        vramSlider->setMinimum(100);
        vramSlider->setMaximum(10000);
        vramSlider->setSingleStep(1);
        vramSlider->setOrientation(Qt::Horizontal);
        vramSlider->setTickPosition(QSlider::TicksBelow);
        gridLayout->addWidget(vramSlider, 2, 1, 1, 1);

        vramLabel = new QLabel(groupBox);
        vramLabel->setObjectName(QString("vramLabel"));
	vramLabel->setToolTip("(MB)");
	vramLabel->setText("VRAM Cache");
        gridLayout->addWidget(vramLabel, 2, 0, 1, 1);

	return groupBox;
}

QGroupBox* VolumeInspector::makeVolPropBox()
{
	QGroupBox* groupBox = new QGroupBox("Volume Properties");
	QGridLayout* gridLayout = new QGridLayout;
	groupBox->setLayout( gridLayout );

	QLabel* scaleLabel = new QLabel(groupBox);
        scaleLabel->setObjectName(QString("scaleLabel"));
	scaleLabel->setText("Scale:");
	gridLayout->addWidget(scaleLabel, 0, 0, 1, 1);

	scaleEdit = new QLineEdit(groupBox);
        scaleEdit->setObjectName(QString("scaleEdit"));
	gridLayout->addWidget(scaleEdit, 0, 1, 1, 1);
       

        QLabel* resolutionLabel = new QLabel(groupBox);
        resolutionLabel->setObjectName(QString("resolutionLabel"));
	resolutionLabel->setToolTip("X, Y, Z");
        resolutionLabel->setText("Pixel Resolution:");
        gridLayout->addWidget(resolutionLabel, 1, 0, 1, 1);

        resolutionEdit = new QLineEdit(groupBox);
        resolutionEdit->setObjectName(QString("resolutionEdit"));
	resolutionEdit->setToolTip("X,Y,Z");
        gridLayout->addWidget(resolutionEdit, 1, 1, 1, 1);


        QLabel* extentLabel = new QLabel(groupBox);
        extentLabel->setObjectName(QString("extentLabel"));
        extentLabel->setToolTip("(in pixels)");
        extentLabel->setText("Data Extent:");
        gridLayout->addWidget(extentLabel, 2, 0, 1, 1);

        extentEdit = new QLineEdit(groupBox);
        extentEdit->setObjectName(QString("extentEdit"));
        gridLayout->addWidget(extentEdit, 2, 1, 1, 1);


	QLabel* lengthLabel = new QLabel(groupBox);
        lengthLabel->setObjectName(QString("lengthLabel"));
        lengthLabel->setToolTip("(in pixels)");
        lengthLabel->setText("Chunk Size:");
        gridLayout->addWidget(lengthLabel, 3, 0, 1, 1);

        sizeSlider = new QSlider(groupBox);
        sizeSlider->setObjectName(QString("sizeSlider"));
        sizeSlider->setMinimum(8);
        sizeSlider->setMaximum(256);
        sizeSlider->setSingleStep(1);
        sizeSlider->setOrientation(Qt::Horizontal);
        sizeSlider->setTickPosition(QSlider::TicksBelow);
        gridLayout->addWidget(sizeSlider, 3, 1, 1, 1);

        sizeLabel = new QLabel(groupBox);
        sizeLabel->setObjectName(QString("sizeLabel"));
	sizeLabel->setText("size");
	gridLayout->addWidget(sizeLabel, 4, 1, 1, 1);

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
	const string & my_name = (*OmVolume::Instance()).GetName();
	nameEdit->setText(QString::fromStdString(my_name));
	nameEdit->setMinimumWidth(200);

	const string & my_notes = (*OmVolume::Instance()).GetNote();
	notesEdit->setPlainText(QString::fromStdString(my_notes));

	Vector3 < float >scale = OmVolume::GetScale();
	scaleEdit->setText("[" + QString::number(scale.x) + " " + QString::number(scale.y) + " " +
			   QString::number(scale.z) + "]");

	Vector3 < float >res = OmVolume::GetDataResolution();
	resolutionEdit->setText("[" + QString::number(res.x) + " " + QString::number(res.y) + " " +
				QString::number(res.z) + "]");

	Vector3i dimen = OmVolume::GetDataDimensions();
	extentEdit->setText("[" + QString::number(dimen.x) + " " + QString::number(dimen.y) + " " +
			    QString::number(dimen.z) + "]");

	int my_chunk_size = OmVolume::GetChunkDimension();
	sizeSlider->setSliderPosition(my_chunk_size / 2);
	sizeLabel->setNum(my_chunk_size);

	ramSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT)));
	ramSizeLabel->setNum(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT)));

	vramSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT)));
	vramSizeLabel->setNum(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT)));
}

void VolumeInspector::on_nameEdit_editingFinished()
{
	OmVolume::Instance()->SetName(nameEdit->text().toStdString());
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
	OmVolume::Instance()->SetScale(scale_vec);
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

void VolumeInspector::on_notesEdit_textChanged()
{
	OmVolume::Instance()->SetNote(notesEdit->toPlainText().toStdString());
}

void VolumeInspector::on_sizeSlider_valueChanged()
{
	sizeLabel->setNum(sizeSlider->value() * 2);
	OmVolume::SetChunkDimension(sizeSlider->value() * 2);
}

void VolumeInspector::on_ramSlider_valueChanged()
{
	OmPreferences::SetFloat(OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT, (ramSlider->value() * 1.0));
	ramSizeLabel->setNum(ramSlider->value());
}

void VolumeInspector::on_vramSlider_valueChanged()
{
	OmPreferences::SetFloat(OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT, (vramSlider->value() * 1.0));
	vramSizeLabel->setNum(vramSlider->value());
}
