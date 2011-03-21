#ifndef VOL_INSPECTOR_H
#define VOL_INSPECTOR_H

#include "system/omConnect.hpp"
#include "project/omProject.h"
#include "volume/omMipVolume.h"

#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>

class OmVolInspector : public QGroupBox {
Q_OBJECT

private Q_SLOTS:
    void apply()
    {
        const Vector3f dims(mResX->text().toFloat(),
                            mResY->text().toFloat(),
                            mResZ->text().toFloat());

        mVol.Coords().SetDataResolution(dims);
    }

private:
    OmMipVolume& mVol;
    QGridLayout* mGrid;
    QLineEdit* mResX;
    QLineEdit* mResY;
    QLineEdit* mResZ;

public:
    virtual ~OmVolInspector()
    {}

    OmVolInspector(OmMipVolume& vol, QWidget * parent)
        : QGroupBox(parent)
        , mVol(vol)
    {
        this->setTitle("Volume");

        const Vector3i dims = mVol.Coords().GetDataDimensions();

        const QString extStr =
            QString("%1 x %2 x %3").arg(dims.x).arg(dims.y).arg(dims.z);
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

        Vector3f resf = mVol.Coords().GetDataResolution();

        QLineEdit * res = new QLineEdit(this);
        mResX = res;
        res->setText(QString::number(resf.x));
        mGrid->addWidget(res, 1, 1);

        mResY = res = new QLineEdit(this);
        res->setText(QString::number(resf.y));
        mGrid->addWidget(res, 2, 1);

        mResZ = res = new QLineEdit(this);
        res->setText(QString::number(resf.z));
        mGrid->addWidget(res, 3, 1);

        QPushButton * apply = new QPushButton("Apply");
        om::connect(apply, SIGNAL(clicked()), this, SLOT(apply()));
        mGrid->addWidget(apply, 4, 1);
    }
};

#endif
