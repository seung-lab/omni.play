#include "gui/inspectors/volInspector.h" 
#include <QLineEdit>
#include <QPushButton>

OmVolInspector::OmVolInspector(OmVolume * vol, QWidget * parent) : QGroupBox(parent), mVol(vol)
{
	this->setTitle("Volume");

        Vector3i dims = vol->GetDataDimensions();
        QString extStr = QString("%1 x %2 x %3") .arg(dims.x) .arg(dims.y) .arg(dims.z);
        mGrid = new QGridLayout(this);

        QLabel *labelVolume = new QLabel(this);
        labelVolume->setText("Volume extents:");
        mGrid->addWidget(labelVolume, 0, 0);

        QLabel *labelVolumeNums = new QLabel(this);
        labelVolumeNums->setText(extStr);
        mGrid->addWidget(labelVolumeNums, 0, 1);

	// Data
        labelVolume = new QLabel(this);
        labelVolume->setText("X Resolution:");
        mGrid->addWidget(labelVolume, 1, 0);

        labelVolume = new QLabel(this);
        labelVolume->setText("Y Resolution:");
        mGrid->addWidget(labelVolume, 2, 0);

        labelVolume = new QLabel(this);
        labelVolume->setText("Z Resolution:");
        mGrid->addWidget(labelVolume, 3, 0);

	Vector3f resf = mVol->GetDataResolution(); 

        QLineEdit * res = new QLineEdit(this);
        res->setText(QString::number(resf.x));
        connect(res, SIGNAL(editingFinished()),
                this, SLOT(xChanged()));
        mGrid->addWidget(res, 1, 1);

        res = new QLineEdit(this);
        res->setText(QString::number(resf.y));
        connect(res, SIGNAL(editingFinished()),
                this, SLOT(yChanged()));
        mGrid->addWidget(res, 2, 1);

        res = new QLineEdit(this);
        res->setText(QString::number(resf.z));
        connect(res, SIGNAL(editingFinished()),
                this, SLOT(zChanged()));
        mGrid->addWidget(res, 3, 1);

	QPushButton * apply = new QPushButton("Apply");
        connect(res, SIGNAL(clicked()),
                this, SLOT(apply()));
	mGrid->addWidget(apply, 4, 1);
}

OmVolInspector::~OmVolInspector()
{
}

