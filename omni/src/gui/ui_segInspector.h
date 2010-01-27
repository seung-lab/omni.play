/********************************************************************************
** Form generated from reading ui file 'segInspector.ui'
**
** Created: Mon Jun 1 12:42:15 2009
**      by: Qt User Interface Compiler version 4.5.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SEGINSPECTOR_H
#define UI_SEGINSPECTOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
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
#include <QtGui/QScrollArea>

QT_BEGIN_NAMESPACE class Ui_segInspector {
 public:
	QVBoxLayout * verticalLayout_3;
	QGroupBox *groupBox;
	QGridLayout *gridLayout_2;
	QHBoxLayout *horizontalLayout_4;
	QSpacerItem *horizontalSpacer_4;
	QGridLayout *gridLayout;
	QLabel *nameLabel;
	QLineEdit *nameEdit;
	QLabel *directoryLabel;
	QLineEdit *directoryEdit;
	QPushButton *browseButton;
	QLabel *patternLabel;
	QLineEdit *patternEdit;
	QListWidget *listWidget;
	QSpacerItem *horizontalSpacer_3;
	QGroupBox *groupBox_2;
	QHBoxLayout *horizontalLayout_2;
	QSpacerItem *horizontalSpacer;
	QVBoxLayout *verticalLayout;
	QHBoxLayout *horizontalLayout;
	QComboBox *buildComboBox;
	QPushButton *buildButton;
	QPushButton *exportButton;
	QSpacerItem *horizontalSpacer_2;
	QFrame *addImagesFrame;
	QHBoxLayout *horizontalLayout_5;
	QSpacerItem *horizontalSpacer_5;
	QVBoxLayout *verticalLayout_2;
	QPushButton *addSegmentButton;
	QSpacerItem *horizontalSpacer_6;
	QGroupBox *groupBox_3;
	QHBoxLayout *horizontalLayout_3;
	QPlainTextEdit *notesEdit;
	QToolButton *toolButton;
	QScrollArea *scrollArea;
	QVBoxLayout *segLayout;
	QGroupBox *segBox;

	void setupUi(QWidget * segInspector) {
		if (segInspector->objectName().isEmpty())
			segInspector->setObjectName(QString::fromUtf8("segInspector"));
		segInspector->resize(298, 680);
		QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(segInspector->sizePolicy().hasHeightForWidth());
		segInspector->setSizePolicy(sizePolicy);
		verticalLayout_3 = new QVBoxLayout(segInspector);
		verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
		groupBox = new QGroupBox(segInspector);
		groupBox->setObjectName(QString::fromUtf8("groupBox"));
		gridLayout_2 = new QGridLayout(groupBox);
		gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
		horizontalLayout_4 = new QHBoxLayout();
		horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
		horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout_4->addItem(horizontalSpacer_4);

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
		QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
		sizePolicy1.setHorizontalStretch(0);
		sizePolicy1.setVerticalStretch(0);
		sizePolicy1.setHeightForWidth(listWidget->sizePolicy().hasHeightForWidth());
		listWidget->setSizePolicy(sizePolicy1);
		listWidget->setMaximumSize(QSize(16777215, 85));

		gridLayout->addWidget(listWidget, 4, 1, 1, 1);

		horizontalLayout_4->addLayout(gridLayout);

		horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout_4->addItem(horizontalSpacer_3);

		gridLayout_2->addLayout(horizontalLayout_4, 0, 0, 1, 1);

		verticalLayout_3->addWidget(groupBox);

		groupBox_2 = new QGroupBox(segInspector);
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

		verticalLayout->addLayout(horizontalLayout);

		exportButton = new QPushButton(groupBox_2);
		exportButton->setObjectName(QString::fromUtf8("exportButton"));

		verticalLayout->addWidget(exportButton);

		horizontalLayout_2->addLayout(verticalLayout);

		horizontalSpacer_2 = new QSpacerItem(29, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout_2->addItem(horizontalSpacer_2);

		verticalLayout_3->addWidget(groupBox_2);

		addImagesFrame = new QFrame(segInspector);
		addImagesFrame->setObjectName(QString::fromUtf8("addImagesFrame"));
		addImagesFrame->setFrameShape(QFrame::StyledPanel);
		addImagesFrame->setFrameShadow(QFrame::Raised);
		horizontalLayout_5 = new QHBoxLayout(addImagesFrame);
		horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
		horizontalSpacer_5 = new QSpacerItem(39, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout_5->addItem(horizontalSpacer_5);

		verticalLayout_2 = new QVBoxLayout();
		verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
		addSegmentButton = new QPushButton(addImagesFrame);
		addSegmentButton->setObjectName(QString::fromUtf8("addSegmentButton"));

		verticalLayout_2->addWidget(addSegmentButton);

		horizontalLayout_5->addLayout(verticalLayout_2);

		horizontalSpacer_6 = new QSpacerItem(39, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout_5->addItem(horizontalSpacer_6);

		verticalLayout_3->addWidget(addImagesFrame);

		groupBox_3 = new QGroupBox(segInspector);
		groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
		QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
		sizePolicy2.setHorizontalStretch(0);
		sizePolicy2.setVerticalStretch(0);
		sizePolicy2.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
		groupBox_3->setSizePolicy(sizePolicy2);
		horizontalLayout_3 = new QHBoxLayout(groupBox_3);
		horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
		notesEdit = new QPlainTextEdit(groupBox_3);
		notesEdit->setObjectName(QString::fromUtf8("notesEdit"));

		horizontalLayout_3->addWidget(notesEdit);
		verticalLayout_3->addWidget(groupBox_3);

		toolButton = new QToolButton(segInspector);
		toolButton->setObjectName(QString::fromUtf8("toolButton"));

		verticalLayout_3->addWidget(toolButton);

		retranslateUi(segInspector);

		QMetaObject::connectSlotsByName(segInspector);
	}			// setupUi

	void retranslateUi(QWidget * segInspector) {
		segInspector->setWindowTitle(QString());
		groupBox->setTitle(QApplication::translate("segInspector", "Source", 0, QApplication::UnicodeUTF8));
		nameLabel->setText(QApplication::translate("segInspector", "Name:", 0, QApplication::UnicodeUTF8));
		nameEdit->setText(QString());
		directoryLabel->
		    setText(QApplication::translate("segInspector", "Directory:", 0, QApplication::UnicodeUTF8));
		browseButton->setText(QApplication::translate("segInspector", "Browse", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
		patternLabel->
		    setToolTip(QApplication::
			       translate("segInspector", "(i.e. data.%d.tif)", 0, QApplication::UnicodeUTF8));
#endif				// QT_NO_TOOLTIP
		patternLabel->
		    setText(QApplication::translate("segInspector", "Pattern:", 0, QApplication::UnicodeUTF8));
		groupBox_2->setTitle(QApplication::translate("segInspector", "Actions", 0, QApplication::UnicodeUTF8));
		buildComboBox->clear();
		buildComboBox->insertItems(0, QStringList()
					   << QApplication::translate("segInspector", "Data", 0,
								      QApplication::UnicodeUTF8)
					   << QApplication::translate("segInspector", "Mesh", 0,
								      QApplication::UnicodeUTF8)
					   << QApplication::translate("segInspector", "Data & Mesh", 0,
								      QApplication::UnicodeUTF8)
		    );
		buildButton->setText(QApplication::translate("segInspector", "Build", 0, QApplication::UnicodeUTF8));
		exportButton->setText(QApplication::translate("segInspector", "Export", 0, QApplication::UnicodeUTF8));
		addSegmentButton->
		    setText(QApplication::translate("segInspector", "Add Segment", 0, QApplication::UnicodeUTF8));
		groupBox_3->setTitle(QApplication::translate("segInspector", "Notes", 0, QApplication::UnicodeUTF8));
		toolButton->setText(QApplication::translate("segInspector", "...", 0, QApplication::UnicodeUTF8));
		Q_UNUSED(segInspector);
	}			// retranslateUi
};

namespace Ui {
	class segInspector:public Ui_segInspector {
	};
} // namespace Ui QT_END_NAMESPACE
#endif				// UI_SEGINSPECTOR_H
