/********************************************************************************
** Form generated from reading ui file 'preferences2d.ui'
**
** Created: Mon May 11 01:32:35 2009
**      by: Qt User Interface Compiler version 4.5.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PREFERENCES2D_H
#define UI_PREFERENCES2D_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_preferences2d
{
public:
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *alphaLabel;
    QLabel *transparencySliderLabel;
    QSlider *transparencySlider;
    QCheckBox *infoCheckBox;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QLabel *volLabel;
    QLineEdit *volEdit;
    QLabel *cacheLabel;
    QLineEdit *tileEdit;
    QLabel *depthLabel;
    QLineEdit *depthEdit;
    QLabel *sideLabel;
    QLineEdit *sideEdit;
    QLabel *mipLabel;
    QLineEdit *mipEdit;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *horizontalSpacer_7;
    QSpacerItem *horizontalSpacer_8;
    QSpacerItem *horizontalSpacer_9;
    QSpacerItem *horizontalSpacer_10;

    void setupUi(QWidget *preferences2d)
    {
        if (preferences2d->objectName().isEmpty())
            preferences2d->setObjectName(QString::fromUtf8("preferences2d"));
        preferences2d->resize(389, 681);
        gridLayout_3 = new QGridLayout(preferences2d);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        groupBox = new QGroupBox(preferences2d);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        alphaLabel = new QLabel(groupBox);
        alphaLabel->setObjectName(QString::fromUtf8("alphaLabel"));

        horizontalLayout->addWidget(alphaLabel);

        transparencySliderLabel = new QLabel(groupBox);
        transparencySliderLabel->setObjectName(QString::fromUtf8("transparencySliderLabel"));
        transparencySliderLabel->setMinimumSize(QSize(40, 0));
        transparencySliderLabel->setMaximumSize(QSize(25, 16777215));

        horizontalLayout->addWidget(transparencySliderLabel);


        verticalLayout->addLayout(horizontalLayout);

        transparencySlider = new QSlider(groupBox);
        transparencySlider->setObjectName(QString::fromUtf8("transparencySlider"));
        transparencySlider->setMaximum(50);
        transparencySlider->setOrientation(Qt::Horizontal);
        transparencySlider->setTickPosition(QSlider::TicksBelow);
        transparencySlider->setTickInterval(5);

        verticalLayout->addWidget(transparencySlider);

        infoCheckBox = new QCheckBox(groupBox);
        infoCheckBox->setObjectName(QString::fromUtf8("infoCheckBox"));

        verticalLayout->addWidget(infoCheckBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        gridLayout_3->addWidget(groupBox, 1, 0, 1, 1);

        groupBox_2 = new QGroupBox(preferences2d);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        volLabel = new QLabel(groupBox_2);
        volLabel->setObjectName(QString::fromUtf8("volLabel"));

        gridLayout_2->addWidget(volLabel, 0, 1, 1, 1);

        volEdit = new QLineEdit(groupBox_2);
        volEdit->setObjectName(QString::fromUtf8("volEdit"));

        gridLayout_2->addWidget(volEdit, 0, 2, 1, 1);

        cacheLabel = new QLabel(groupBox_2);
        cacheLabel->setObjectName(QString::fromUtf8("cacheLabel"));

        gridLayout_2->addWidget(cacheLabel, 1, 1, 1, 1);

        tileEdit = new QLineEdit(groupBox_2);
        tileEdit->setObjectName(QString::fromUtf8("tileEdit"));

        gridLayout_2->addWidget(tileEdit, 1, 2, 1, 1);

        depthLabel = new QLabel(groupBox_2);
        depthLabel->setObjectName(QString::fromUtf8("depthLabel"));

        gridLayout_2->addWidget(depthLabel, 2, 1, 1, 1);

        depthEdit = new QLineEdit(groupBox_2);
        depthEdit->setObjectName(QString::fromUtf8("depthEdit"));

        gridLayout_2->addWidget(depthEdit, 2, 2, 1, 1);

        sideLabel = new QLabel(groupBox_2);
        sideLabel->setObjectName(QString::fromUtf8("sideLabel"));

        gridLayout_2->addWidget(sideLabel, 3, 1, 1, 1);

        sideEdit = new QLineEdit(groupBox_2);
        sideEdit->setObjectName(QString::fromUtf8("sideEdit"));

        gridLayout_2->addWidget(sideEdit, 3, 2, 1, 1);

        mipLabel = new QLabel(groupBox_2);
        mipLabel->setObjectName(QString::fromUtf8("mipLabel"));

        gridLayout_2->addWidget(mipLabel, 4, 1, 1, 1);

        mipEdit = new QLineEdit(groupBox_2);
        mipEdit->setObjectName(QString::fromUtf8("mipEdit"));

        gridLayout_2->addWidget(mipEdit, 4, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 1, 0, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_3, 2, 0, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_4, 3, 0, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_5, 4, 0, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_6, 1, 3, 1, 1);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_7, 0, 3, 1, 1);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_8, 2, 3, 1, 1);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_9, 3, 3, 1, 1);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_10, 4, 3, 1, 1);


        gridLayout_3->addWidget(groupBox_2, 0, 0, 1, 1);


        retranslateUi(preferences2d);
        QObject::connect(transparencySlider, SIGNAL(valueChanged(int)), transparencySliderLabel, SLOT(setNum(int)));

        QMetaObject::connectSlotsByName(preferences2d);
    } // setupUi

    void retranslateUi(QWidget *preferences2d)
    {
        preferences2d->setWindowTitle(QString());
        groupBox->setTitle(QApplication::translate("preferences2d", "Display Properties", 0, QApplication::UnicodeUTF8));
        alphaLabel->setText(QApplication::translate("preferences2d", "Alpha:", 0, QApplication::UnicodeUTF8));
        transparencySliderLabel->setText(QApplication::translate("preferences2d", "0", 0, QApplication::UnicodeUTF8));
        infoCheckBox->setText(QApplication::translate("preferences2d", "Show Information", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        groupBox_2->setToolTip(QApplication::translate("preferences2d", "(mB)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        groupBox_2->setTitle(QApplication::translate("preferences2d", "Cache Properties", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        volLabel->setToolTip(QApplication::translate("preferences2d", "(MB)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        volLabel->setText(QApplication::translate("preferences2d", "Volume Cache Size:", 0, QApplication::UnicodeUTF8));
        volEdit->setText(QString());
#ifndef QT_NO_TOOLTIP
        cacheLabel->setToolTip(QApplication::translate("preferences2d", "(MB)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cacheLabel->setText(QApplication::translate("preferences2d", "Tile Cache Size:", 0, QApplication::UnicodeUTF8));
        tileEdit->setText(QString());
#ifndef QT_NO_TOOLTIP
        depthLabel->setToolTip(QApplication::translate("preferences2d", "(slices)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        depthLabel->setText(QApplication::translate("preferences2d", "Depth Caching:", 0, QApplication::UnicodeUTF8));
        depthEdit->setText(QString());
#ifndef QT_NO_TOOLTIP
        sideLabel->setToolTip(QApplication::translate("preferences2d", "(tiles)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        sideLabel->setText(QApplication::translate("preferences2d", "Side Caching:", 0, QApplication::UnicodeUTF8));
        sideEdit->setText(QString());
#ifndef QT_NO_TOOLTIP
        mipLabel->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        mipLabel->setText(QApplication::translate("preferences2d", "Mip Level Caching:", 0, QApplication::UnicodeUTF8));
        mipEdit->setText(QString());
        Q_UNUSED(preferences2d);
    } // retranslateUi

};

namespace Ui {
    class preferences2d: public Ui_preferences2d {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCES2D_H
