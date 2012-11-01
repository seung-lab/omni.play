#pragma once

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

private:
    OmMipVolume& vol_;
    QGridLayout* grid_;
    QLineEdit* resX_;
    QLineEdit* resY_;
    QLineEdit* resZ_;

private Q_SLOTS:
    void apply()
    {
        const Vector3i dims(resX_->text().toFloat(),
                            resY_->text().toFloat(),
                            resZ_->text().toFloat());

        vol_.Coords().SetResolution(dims);
    }

public:
    OmVolInspector(OmMipVolume& vol, QWidget* parent)
        : QGroupBox(parent)
        , vol_(vol)
    {
        setTitle("Volume Resolution");

        const Vector3i dims = vol_.Coords().DataDimensions();

        const QString extStr = QString("%1 x %2 x %3").arg(dims.x).arg(dims.y).arg(dims.z);
        grid_ = new QGridLayout(this);

        QLabel* labelVolume = new QLabel(this);
        labelVolume->setText("Chunk size:");
        grid_->addWidget(labelVolume, 0, 0);

        QLabel* labelVolumeNums = new QLabel(this);
        labelVolumeNums->setText(extStr);
        grid_->addWidget(labelVolumeNums, 0, 1);

        // Data
        labelVolume = new QLabel(this);
        labelVolume->setText("X Resolution:");
        grid_->addWidget(labelVolume, 1, 0);

        labelVolume = new QLabel(this);
        labelVolume->setText("Y Resolution:");
        grid_->addWidget(labelVolume, 2, 0);

        labelVolume = new QLabel(this);
        labelVolume->setText("Z Resolution:");
        grid_->addWidget(labelVolume, 3, 0);

        const Vector3i resf = vol_.Coords().GetResolution();

        resX_ = new QLineEdit(this);
        resX_->setText(QString::number(resf.x));
        grid_->addWidget(resX_, 1, 1);

        resY_ = new QLineEdit(this);
        resY_->setText(QString::number(resf.y));
        grid_->addWidget(resY_, 2, 1);

        resZ_ = new QLineEdit(this);
        resZ_->setText(QString::number(resf.z));
        grid_->addWidget(resZ_, 3, 1);

        QPushButton* apply = new QPushButton("Apply");
        om::connect(apply, SIGNAL(clicked()), this, SLOT(apply()));
        grid_->addWidget(apply, 4, 1);
    }
};

