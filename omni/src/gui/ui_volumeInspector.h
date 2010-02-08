/********************************************************************************
** Form generated from reading ui file 'volumeInspector.ui'
**
** Created: Thu Jun 4 13:40:33 2009
**      by: Qt User Interface Compiler version 4.5.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_VOLUMEINSPECTOR_H
#define UI_VOLUMEINSPECTOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_volumeInspector
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QLabel *scaleLabel;
    QLineEdit *scaleEdit;
    QLabel *resolutionLabel;
    QLineEdit *resolutionEdit;
    QLabel *extentLabel;
    QLineEdit *extentEdit;
    QLabel *lengthLabel;
    QSlider *sizeSlider;
    QLabel *sizeLabel;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_3;
    QLabel *ramLabel;
    QSlider *ramSlider;
    QLabel *ramSizeLabel;
    QSlider *vramSlider;
    QLabel *vramSizeLabel;
    QLabel *vramLabel;
    QFrame *addImagesFrame;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_4;
    QVBoxLayout *verticalLayout;
    QPushButton *addChannelButton;
    QPushButton *addSegmentationButton;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_4;
    QPlainTextEdit *notesEdit;
    QToolButton *toolButton;

    void setupUi(QWidget *volumeInspector)
    {
        if (volumeInspector->objectName().isEmpty())
            volumeInspector->setObjectName(QString::fromUtf8("volumeInspector"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(volumeInspector->sizePolicy().hasHeightForWidth());
        volumeInspector->setSizePolicy(sizePolicy);
        verticalLayout_2 = new QVBoxLayout(volumeInspector);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox = new QGroupBox(volumeInspector);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        nameLabel = new QLabel(groupBox);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        gridLayout_2->addWidget(nameLabel, 0, 0, 1, 1);

        nameEdit = new QLineEdit(groupBox);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));

        gridLayout_2->addWidget(nameEdit, 0, 1, 1, 1);


        verticalLayout_2->addWidget(groupBox);

        groupBox_2 = new QGroupBox(volumeInspector);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        scaleLabel = new QLabel(groupBox_2);
        scaleLabel->setObjectName(QString::fromUtf8("scaleLabel"));

        gridLayout->addWidget(scaleLabel, 0, 0, 1, 1);

        scaleEdit = new QLineEdit(groupBox_2);
        scaleEdit->setObjectName(QString::fromUtf8("scaleEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scaleEdit->sizePolicy().hasHeightForWidth());
        scaleEdit->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(scaleEdit, 0, 1, 1, 1);

        resolutionLabel = new QLabel(groupBox_2);
        resolutionLabel->setObjectName(QString::fromUtf8("resolutionLabel"));

        gridLayout->addWidget(resolutionLabel, 1, 0, 1, 1);

        resolutionEdit = new QLineEdit(groupBox_2);
        resolutionEdit->setObjectName(QString::fromUtf8("resolutionEdit"));
        sizePolicy1.setHeightForWidth(resolutionEdit->sizePolicy().hasHeightForWidth());
        resolutionEdit->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(resolutionEdit, 1, 1, 1, 1);

        extentLabel = new QLabel(groupBox_2);
        extentLabel->setObjectName(QString::fromUtf8("extentLabel"));

        gridLayout->addWidget(extentLabel, 2, 0, 1, 1);

        extentEdit = new QLineEdit(groupBox_2);
        extentEdit->setObjectName(QString::fromUtf8("extentEdit"));
        sizePolicy1.setHeightForWidth(extentEdit->sizePolicy().hasHeightForWidth());
        extentEdit->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(extentEdit, 2, 1, 1, 1);

        lengthLabel = new QLabel(groupBox_2);
        lengthLabel->setObjectName(QString::fromUtf8("lengthLabel"));

        gridLayout->addWidget(lengthLabel, 3, 0, 1, 1);

        sizeSlider = new QSlider(groupBox_2);
        sizeSlider->setObjectName(QString::fromUtf8("sizeSlider"));
        sizeSlider->setMinimum(8);
        sizeSlider->setMaximum(256);
        sizeSlider->setSingleStep(1);
        sizeSlider->setOrientation(Qt::Horizontal);
        sizeSlider->setTickPosition(QSlider::TicksBelow);

        gridLayout->addWidget(sizeSlider, 3, 1, 1, 1);

        sizeLabel = new QLabel(groupBox_2);
        sizeLabel->setObjectName(QString::fromUtf8("sizeLabel"));

        gridLayout->addWidget(sizeLabel, 4, 1, 1, 1);


        verticalLayout_2->addWidget(groupBox_2);

        groupBox_4 = new QGroupBox(volumeInspector);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        gridLayout_3 = new QGridLayout(groupBox_4);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        ramLabel = new QLabel(groupBox_4);
        ramLabel->setObjectName(QString::fromUtf8("ramLabel"));

        gridLayout_3->addWidget(ramLabel, 0, 0, 1, 1);

        ramSlider = new QSlider(groupBox_4);
        ramSlider->setObjectName(QString::fromUtf8("ramSlider"));
        ramSlider->setMinimum(100);
        ramSlider->setMaximum(10000);
        ramSlider->setSingleStep(1);
        ramSlider->setOrientation(Qt::Horizontal);
        ramSlider->setTickPosition(QSlider::TicksBelow);

        gridLayout_3->addWidget(ramSlider, 0, 1, 1, 1);

        ramSizeLabel = new QLabel(groupBox_4);
        ramSizeLabel->setObjectName(QString::fromUtf8("ramSizeLabel"));

        gridLayout_3->addWidget(ramSizeLabel, 1, 1, 1, 1);

        vramSlider = new QSlider(groupBox_4);
        vramSlider->setObjectName(QString::fromUtf8("vramSlider"));
        vramSlider->setMinimum(100);
        vramSlider->setMaximum(10000);
        vramSlider->setSingleStep(1);
        vramSlider->setOrientation(Qt::Horizontal);
        vramSlider->setTickPosition(QSlider::TicksBelow);

        gridLayout_3->addWidget(vramSlider, 2, 1, 1, 1);

        vramSizeLabel = new QLabel(groupBox_4);
        vramSizeLabel->setObjectName(QString::fromUtf8("vramSizeLabel"));

        gridLayout_3->addWidget(vramSizeLabel, 3, 1, 1, 1);

        vramLabel = new QLabel(groupBox_4);
        vramLabel->setObjectName(QString::fromUtf8("vramLabel"));

        gridLayout_3->addWidget(vramLabel, 2, 0, 1, 1);


        verticalLayout_2->addWidget(groupBox_4);

        addImagesFrame = new QFrame(volumeInspector);
        addImagesFrame->setObjectName(QString::fromUtf8("addImagesFrame"));
        addImagesFrame->setFrameShape(QFrame::StyledPanel);
        addImagesFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(addImagesFrame);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer_4 = new QSpacerItem(39, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        addChannelButton = new QPushButton(addImagesFrame);
        addChannelButton->setObjectName(QString::fromUtf8("addChannelButton"));

        verticalLayout->addWidget(addChannelButton);

        addSegmentationButton = new QPushButton(addImagesFrame);
        addSegmentationButton->setObjectName(QString::fromUtf8("addSegmentationButton"));

        verticalLayout->addWidget(addSegmentationButton);


        horizontalLayout_3->addLayout(verticalLayout);

        horizontalSpacer_3 = new QSpacerItem(39, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout_2->addWidget(addImagesFrame);

        groupBox_3 = new QGroupBox(volumeInspector);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy2);
        horizontalLayout_4 = new QHBoxLayout(groupBox_3);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        notesEdit = new QPlainTextEdit(groupBox_3);
        notesEdit->setObjectName(QString::fromUtf8("notesEdit"));

        horizontalLayout_4->addWidget(notesEdit);


        verticalLayout_2->addWidget(groupBox_3);

        toolButton = new QToolButton(volumeInspector);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));

        verticalLayout_2->addWidget(toolButton);


        retranslateUi(volumeInspector);

        QMetaObject::connectSlotsByName(volumeInspector);
    } // setupUi

    void retranslateUi(QWidget *volumeInspector)
    {
        volumeInspector->setWindowTitle(QString());
        groupBox->setTitle(QApplication::translate("volumeInspector", "Source Properties", 0, QApplication::UnicodeUTF8));
        nameLabel->setText(QApplication::translate("volumeInspector", "Name:", 0, QApplication::UnicodeUTF8));
        nameEdit->setText(QString());
        groupBox_2->setTitle(QApplication::translate("volumeInspector", "Volume Properties", 0, QApplication::UnicodeUTF8));
        scaleLabel->setText(QApplication::translate("volumeInspector", "Scale:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        resolutionLabel->setToolTip(QApplication::translate("volumeInspector", "X, Y, Z", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        resolutionLabel->setText(QApplication::translate("volumeInspector", "Pixel Resolution:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        resolutionEdit->setToolTip(QApplication::translate("volumeInspector", "X,Y,Z", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        extentLabel->setToolTip(QApplication::translate("volumeInspector", "(in pixels)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        extentLabel->setText(QApplication::translate("volumeInspector", "Data Extent:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        lengthLabel->setToolTip(QApplication::translate("volumeInspector", "(in pixels)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        lengthLabel->setText(QApplication::translate("volumeInspector", "Chunk Size:", 0, QApplication::UnicodeUTF8));
        sizeLabel->setText(QApplication::translate("volumeInspector", "size", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("volumeInspector", "Cache Properties", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ramLabel->setToolTip(QApplication::translate("volumeInspector", "(MB)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ramLabel->setText(QApplication::translate("volumeInspector", "RAM Cache", 0, QApplication::UnicodeUTF8));
        ramSizeLabel->setText(QApplication::translate("volumeInspector", "size", 0, QApplication::UnicodeUTF8));
        vramSizeLabel->setText(QApplication::translate("volumeInspector", "size", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        vramLabel->setToolTip(QApplication::translate("volumeInspector", "(MB)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        vramLabel->setText(QApplication::translate("volumeInspector", "VRAM Cache", 0, QApplication::UnicodeUTF8));
        addChannelButton->setText(QApplication::translate("volumeInspector", "Add Channel", 0, QApplication::UnicodeUTF8));
        addSegmentationButton->setText(QApplication::translate("volumeInspector", "Add Segmentation", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("volumeInspector", "Notes", 0, QApplication::UnicodeUTF8));
        toolButton->setText(QApplication::translate("volumeInspector", "...", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(volumeInspector);
    } // retranslateUi

};

namespace Ui {
    class volumeInspector: public Ui_volumeInspector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VOLUMEINSPECTOR_H
