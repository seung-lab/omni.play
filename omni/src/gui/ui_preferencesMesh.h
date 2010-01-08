/********************************************************************************
** Form generated from reading ui file 'preferencesMesh.ui'
**
** Created: Mon Apr 20 17:09:51 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PREFERENCESMESH_H
#define UI_PREFERENCESMESH_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_preferencesMesh
{
public:
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *tdpLabel;
    QLabel *tdpSliderLabel;
    QSpacerItem *horizontalSpacer;
    QSlider *tdpSlider;
    QHBoxLayout *horizontalLayout_2;
    QLabel *pasLabel;
    QLabel *pasSliderLabel;
    QSpacerItem *horizontalSpacer_2;
    QSlider *pasSlider;
    QHBoxLayout *horizontalLayout;
    QLabel *nsLabel;
    QLabel *nsSliderLabel;
    QSpacerItem *horizontalSpacer_3;
    QSlider *nsSlider;

    void setupUi(QWidget *preferencesMesh)
    {
    if (preferencesMesh->objectName().isEmpty())
        preferencesMesh->setObjectName(QString::fromUtf8("preferencesMesh"));
    preferencesMesh->resize(550, 379);
    gridLayout_2 = new QGridLayout(preferencesMesh);
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    groupBox = new QGroupBox(preferencesMesh);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    gridLayout = new QGridLayout(groupBox);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    tdpLabel = new QLabel(groupBox);
    tdpLabel->setObjectName(QString::fromUtf8("tdpLabel"));

    horizontalLayout_3->addWidget(tdpLabel);

    tdpSliderLabel = new QLabel(groupBox);
    tdpSliderLabel->setObjectName(QString::fromUtf8("tdpSliderLabel"));
    tdpSliderLabel->setMinimumSize(QSize(20, 0));
    tdpSliderLabel->setMaximumSize(QSize(20, 16777215));

    horizontalLayout_3->addWidget(tdpSliderLabel);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_3->addItem(horizontalSpacer);


    gridLayout->addLayout(horizontalLayout_3, 0, 0, 1, 1);

    tdpSlider = new QSlider(groupBox);
    tdpSlider->setObjectName(QString::fromUtf8("tdpSlider"));
    tdpSlider->setMaximum(100);
    tdpSlider->setOrientation(Qt::Horizontal);
    tdpSlider->setTickPosition(QSlider::TicksBelow);
    tdpSlider->setTickInterval(10);

    gridLayout->addWidget(tdpSlider, 1, 0, 1, 1);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    pasLabel = new QLabel(groupBox);
    pasLabel->setObjectName(QString::fromUtf8("pasLabel"));

    horizontalLayout_2->addWidget(pasLabel);

    pasSliderLabel = new QLabel(groupBox);
    pasSliderLabel->setObjectName(QString::fromUtf8("pasSliderLabel"));
    pasSliderLabel->setMinimumSize(QSize(20, 0));
    pasSliderLabel->setMaximumSize(QSize(20, 16777215));

    horizontalLayout_2->addWidget(pasSliderLabel);

    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_2->addItem(horizontalSpacer_2);


    gridLayout->addLayout(horizontalLayout_2, 2, 0, 1, 1);

    pasSlider = new QSlider(groupBox);
    pasSlider->setObjectName(QString::fromUtf8("pasSlider"));
    pasSlider->setMaximum(90);
    pasSlider->setOrientation(Qt::Horizontal);
    pasSlider->setTickPosition(QSlider::TicksBelow);
    pasSlider->setTickInterval(10);

    gridLayout->addWidget(pasSlider, 3, 0, 1, 1);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    nsLabel = new QLabel(groupBox);
    nsLabel->setObjectName(QString::fromUtf8("nsLabel"));

    horizontalLayout->addWidget(nsLabel);

    nsSliderLabel = new QLabel(groupBox);
    nsSliderLabel->setObjectName(QString::fromUtf8("nsSliderLabel"));
    nsSliderLabel->setMinimumSize(QSize(20, 0));
    nsSliderLabel->setMaximumSize(QSize(20, 16777215));

    horizontalLayout->addWidget(nsSliderLabel);

    horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer_3);


    gridLayout->addLayout(horizontalLayout, 4, 0, 1, 1);

    nsSlider = new QSlider(groupBox);
    nsSlider->setObjectName(QString::fromUtf8("nsSlider"));
    nsSlider->setMaximum(100);
    nsSlider->setOrientation(Qt::Horizontal);
    nsSlider->setTickPosition(QSlider::TicksBelow);
    nsSlider->setTickInterval(10);

    gridLayout->addWidget(nsSlider, 5, 0, 1, 1);

    tdpLabel->raise();
    tdpSlider->raise();
    pasLabel->raise();
    pasSliderLabel->raise();
    pasSlider->raise();
    nsLabel->raise();
    nsSliderLabel->raise();
    nsSlider->raise();
    tdpSliderLabel->raise();

    gridLayout_2->addWidget(groupBox, 0, 0, 1, 1);


    retranslateUi(preferencesMesh);
    QObject::connect(tdpSlider, SIGNAL(valueChanged(int)), tdpSliderLabel, SLOT(setNum(int)));
    QObject::connect(nsSlider, SIGNAL(valueChanged(int)), nsSliderLabel, SLOT(setNum(int)));
    QObject::connect(pasSlider, SIGNAL(valueChanged(int)), pasSliderLabel, SLOT(setNum(int)));

    QMetaObject::connectSlotsByName(preferencesMesh);
    } // setupUi

    void retranslateUi(QWidget *preferencesMesh)
    {
    preferencesMesh->setWindowTitle(QString());
    groupBox->setTitle(QApplication::translate("preferencesMesh", "Mesh Options", 0, QApplication::UnicodeUTF8));
    tdpLabel->setText(QApplication::translate("preferencesMesh", "Target Decimation Percentage:", 0, QApplication::UnicodeUTF8));
    tdpSliderLabel->setText(QString());
    pasLabel->setText(QApplication::translate("preferencesMesh", "Preserved Angle Sharpness:", 0, QApplication::UnicodeUTF8));
    pasSliderLabel->setText(QString());
    nsLabel->setText(QApplication::translate("preferencesMesh", "Normal Smoothness:", 0, QApplication::UnicodeUTF8));
    nsSliderLabel->setText(QString());
    Q_UNUSED(preferencesMesh);
    } // retranslateUi

};

namespace Ui {
    class preferencesMesh: public Ui_preferencesMesh {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCESMESH_H
