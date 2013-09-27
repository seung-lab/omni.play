#include "system/omConnect.hpp"
#include "common/logging.h"
#include "filObjectInspector.h"
#include "events/omEvents.h"
#include "utility/dataWrappers.h"

FilObjectInspector::FilObjectInspector(QWidget * parent,
                                       const FilterDataWrapper & fdw )
    : QWidget(parent)
{
    fdw_ = std::make_shared<FilterDataWrapper>(fdw);

    QVBoxLayout* overallContainer = new QVBoxLayout( this );

    QGroupBox* groupBox = new QGroupBox("Filter");
    overallContainer->addWidget( groupBox );

    QGridLayout *grid = new QGridLayout( groupBox );

    grid->addWidget( makeFilterOptionsBox(), 0, 0 );
    grid->addWidget( makeNotesBox(), 1, 0 );
    grid->setRowStretch( 2, 1 );

    set_initial_values();

    om::connect(chanEdit, SIGNAL(editingFinished()),
            this, SLOT(sourceEditChangedChan()));
    om::connect(segEdit, SIGNAL(editingFinished()),
            this, SLOT(sourceEditChangedSeg()));
    om::connect(alphaSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setFilAlpha(int)));
    om::connect(alphaSlider, SIGNAL(sliderReleased()),
            this, SLOT(saveFilterAlphaValue()));
}

void FilObjectInspector::saveFilterAlphaValue()
{}

void FilObjectInspector::setFilAlpha(int alpha)
{
    if(fdw_->isValid()){
        fdw_->getFilter()->SetAlpha((double)alpha / 100.00);
        OmEvents::Redraw2d();
    }
}

void FilObjectInspector::set_initial_values()
{
    if(!fdw_->isValid()){
        return;
    }

    OmFilter2d* filter = fdw_->getFilter();

    alphaSlider->setValue(filter->GetAlpha() * 100);
    chanEdit->setText(QString::number(filter->GetChannelID()));
    segEdit->setText(QString::number(filter->GetSegmentationID()));
}

QGroupBox* FilObjectInspector::makeFilterOptionsBox()
{
    QGroupBox* groupBox = new QGroupBox("Filter Options");
    QGridLayout* gridLayout = new QGridLayout;
    groupBox->setLayout( gridLayout );

    QLabel* chanLabel = new QLabel(groupBox);
    chanLabel->setObjectName(QString::fromUtf8("chanLabel"));
    chanLabel->setText("Source Channel ID:");
    gridLayout->addWidget(chanLabel, 0, 0, 1, 1);

    chanEdit = new QLineEdit(groupBox);
    chanEdit->setObjectName(QString::fromUtf8("chanEdit"));
    gridLayout->addWidget(chanEdit, 0, 1, 1, 1);

    QLabel* segLabel = new QLabel(groupBox);
    segLabel->setObjectName(QString::fromUtf8("segLabel"));
    segLabel->setText("Source Segmentation ID:");
    gridLayout->addWidget(segLabel, 1, 0, 1, 1);

    segEdit = new QLineEdit(groupBox);
    segEdit->setObjectName(QString::fromUtf8("segEdit"));
    gridLayout->addWidget(segEdit, 1, 1, 1, 1);

    QLabel* tagsLabel = new QLabel(groupBox);
    tagsLabel->setObjectName(QString::fromUtf8("tagsLabel"));
    tagsLabel->setText("Tags:");
    gridLayout->addWidget(tagsLabel, 2, 0, 1, 1);

    tagsEdit = new QLineEdit(groupBox);
    tagsEdit->setObjectName(QString::fromUtf8("tagsEdit"));
    gridLayout->addWidget(tagsEdit, 2, 1, 1, 1);

    QLabel* alphaLabel = new QLabel(groupBox);
    alphaLabel->setObjectName(QString::fromUtf8("alphaLabel"));
    alphaLabel->setText("Alpha:");
    gridLayout->addWidget(alphaLabel, 3, 0, 1, 1);

    alphaSlider = new QSlider(Qt::Horizontal, groupBox);
    alphaSlider->setObjectName(QString::fromUtf8("alphaSlider"));
    gridLayout->addWidget(alphaSlider, 3, 1, 1, 1);

    return groupBox;
}

QGroupBox* FilObjectInspector::makeNotesBox()
{
    QGroupBox* groupBox = new QGroupBox("Notes");
    QVBoxLayout* vbox = new QVBoxLayout();
    groupBox->setLayout( vbox );

    notesEdit = new QPlainTextEdit(groupBox);
    notesEdit->setObjectName(QString("notesEdit"));
    vbox->addWidget(notesEdit);

    return groupBox;
}

int FilObjectInspector::getChannelIDtoFilter()
{
    return chanEdit->text().toInt();
}

int FilObjectInspector::GetSegmentationIDtoFilter()
{
    return segEdit->text().toInt();
}

void FilObjectInspector::sourceEditChangedChan()
{
    if(!fdw_->isValid()){
        return;
    }

    fdw_->getFilter()->SetChannel(getChannelIDtoFilter());
}

void FilObjectInspector::sourceEditChangedSeg()
{
    if(!fdw_->isValid()){
        return;
    }

    fdw_->getFilter()->SetSegmentation(GetSegmentationIDtoFilter());
}
