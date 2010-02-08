/********************************************************************************
** Form generated from reading ui file 'chanInspector.ui'
**
** Created: Mon May 11 10:48:36 2009
**      by: Qt User Interface Compiler version 4.5.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CHANINSPECTOR_H
#define UI_CHANINSPECTOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_chanInspector
{
public:
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_3;
    QGridLayout *gridLayout;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QLabel *directoryLabel;
    QLineEdit *directoryEdit;
    QPushButton *browseButton;
    QLabel *patternLabel;
    QLineEdit *patternEdit;
    QListWidget *listWidget;
    QSpacerItem *horizontalSpacer_4;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QComboBox *buildComboBox;
    QPushButton *buildButton;
    QPushButton *exportButton;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_3;
    QPlainTextEdit *notesEdit;
    QToolButton *toolButton;
    QPushButton *addFilterButton;

    void setupUi(QWidget *chanInspector)
    {
        if (chanInspector->objectName().isEmpty())
            chanInspector->setObjectName(QString::fromUtf8("chanInspector"));
        chanInspector->resize(281, 614);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(chanInspector->sizePolicy().hasHeightForWidth());
        chanInspector->setSizePolicy(sizePolicy);
        gridLayout_3 = new QGridLayout(chanInspector);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        groupBox = new QGroupBox(chanInspector);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        nameLabel = new QLabel(groupBox);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        gridLayout->addWidget(nameLabel, 0, 0, 1, 1);

        nameEdit = new QLineEdit(groupBox);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));

        gridLayout->addWidget(nameEdit, 0, 1, 1, 1);

        directoryLabel = new QLabel(groupBox);
        directoryLabel->setObjectName(QString::fromUtf8("directoryLabel"));

        gridLayout->addWidget(directoryLabel, 1, 0, 1, 1);

        directoryEdit = new QLineEdit(groupBox);
        directoryEdit->setObjectName(QString::fromUtf8("directoryEdit"));

        gridLayout->addWidget(directoryEdit, 1, 1, 1, 1);

        browseButton = new QPushButton(groupBox);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));

        gridLayout->addWidget(browseButton, 2, 1, 1, 1);

        patternLabel = new QLabel(groupBox);
        patternLabel->setObjectName(QString::fromUtf8("patternLabel"));

        gridLayout->addWidget(patternLabel, 3, 0, 1, 1);

        patternEdit = new QLineEdit(groupBox);
        patternEdit->setObjectName(QString::fromUtf8("patternEdit"));

        gridLayout->addWidget(patternEdit, 3, 1, 1, 1);

        listWidget = new QListWidget(groupBox);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        sizePolicy.setHeightForWidth(listWidget->sizePolicy().hasHeightForWidth());
        listWidget->setSizePolicy(sizePolicy);
        listWidget->setMaximumSize(QSize(16777215, 85));

        gridLayout->addWidget(listWidget, 4, 1, 1, 1);


        horizontalLayout_4->addLayout(gridLayout);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        gridLayout_2->addLayout(horizontalLayout_4, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_2 = new QGroupBox(chanInspector);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(28, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buildComboBox = new QComboBox(groupBox_2);
        buildComboBox->setObjectName(QString::fromUtf8("buildComboBox"));

        horizontalLayout->addWidget(buildComboBox);

        buildButton = new QPushButton(groupBox_2);
        buildButton->setObjectName(QString::fromUtf8("buildButton"));
        buildButton->setEnabled(true);
        horizontalLayout->addWidget(buildButton);

        addFilterButton = new QPushButton(groupBox_2);
        addFilterButton->setObjectName(QString::fromUtf8("addFilterButton"));
        horizontalLayout->addWidget(addFilterButton);


        verticalLayout->addLayout(horizontalLayout);

        exportButton = new QPushButton(groupBox_2);
        exportButton->setObjectName(QString::fromUtf8("exportButton"));

        verticalLayout->addWidget(exportButton);


        horizontalLayout_2->addLayout(verticalLayout);

        horizontalSpacer_2 = new QSpacerItem(29, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        gridLayout_3->addWidget(groupBox_2, 1, 0, 1, 1);

        groupBox_3 = new QGroupBox(chanInspector);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy1);
        horizontalLayout_3 = new QHBoxLayout(groupBox_3);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        notesEdit = new QPlainTextEdit(groupBox_3);
        notesEdit->setObjectName(QString::fromUtf8("notesEdit"));

        horizontalLayout_3->addWidget(notesEdit);


        gridLayout_3->addWidget(groupBox_3, 2, 0, 1, 1);

        toolButton = new QToolButton(chanInspector);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));

        gridLayout_3->addWidget(toolButton, 3, 0, 1, 1);


        retranslateUi(chanInspector);

        QMetaObject::connectSlotsByName(chanInspector);
    } // setupUi

    void retranslateUi(QWidget *chanInspector)
    {
        chanInspector->setWindowTitle(QString());
        groupBox->setTitle("Source Properties");
        nameLabel->setText("Name:");
        nameEdit->setText(QString());
        directoryLabel->setText("Directory:");
        browseButton->setText("Browse");
#ifndef QT_NO_TOOLTIP
        patternLabel->setToolTip("(i.e. data.%d.tif)");
#endif // QT_NO_TOOLTIP
        patternLabel->setText("Regex:");
        groupBox_2->setTitle("Channel Properties");
        buildComboBox->clear();
        buildComboBox->insertItems(0, QStringList()
         << "Data"
        );
        buildButton->setText("Build");
        addFilterButton->setText("Add Filter");
        exportButton->setText("Export");
        groupBox_3->setTitle("Notes");
        toolButton->setText("...");
        Q_UNUSED(chanInspector);
    } // retranslateUi

};

namespace Ui {
    class chanInspector: public Ui_chanInspector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHANINSPECTOR_H
