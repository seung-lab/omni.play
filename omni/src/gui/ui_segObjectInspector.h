/********************************************************************************
** Form generated from reading ui file 'segObjectInspector.ui'
**
** Created: Mon Apr 6 14:26:28 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SEGOBJECTINSPECTOR_H
#define UI_SEGOBJECTINSPECTOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE class Ui_segObjectInspector {
 public:
	QVBoxLayout * verticalLayout_2;
	QGroupBox *groupBox_2;
	QVBoxLayout *verticalLayout;
	QFormLayout *formLayout;
	QLabel *nameLabel;
	QLineEdit *nameEdit;
	QLabel *nameLabel_2;
	QLineEdit *nameEdit_2;
	QLabel *labelsLabel;
	QLineEdit *labelsEdit;
	QLabel *colorLabel;
	QPushButton *colorButton;
	QGroupBox *groupBox;
	QHBoxLayout *horizontalLayout;
	QPlainTextEdit *notesEdit;

	void setupUi(QWidget * segObjectInspector) {
		if (segObjectInspector->objectName().isEmpty())
			segObjectInspector->setObjectName(QString::fromUtf8("segObjectInspector"));
		segObjectInspector->resize(318, 699);
		verticalLayout_2 = new QVBoxLayout(segObjectInspector);
		verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
		groupBox_2 = new QGroupBox(segObjectInspector);
		groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
		verticalLayout = new QVBoxLayout(groupBox_2);
		verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
		formLayout = new QFormLayout();
		formLayout->setObjectName(QString::fromUtf8("formLayout"));
		nameLabel = new QLabel(groupBox_2);
		nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

		formLayout->setWidget(0, QFormLayout::LabelRole, nameLabel);

		nameEdit = new QLineEdit(groupBox_2);
		nameEdit->setObjectName(QString::fromUtf8("nameEdit"));

		formLayout->setWidget(0, QFormLayout::FieldRole, nameEdit);

		nameLabel_2 = new QLabel(groupBox_2);
		nameLabel_2->setObjectName(QString::fromUtf8("nameLabel_2"));

		formLayout->setWidget(1, QFormLayout::LabelRole, nameLabel_2);

		nameEdit_2 = new QLineEdit(groupBox_2);
		nameEdit_2->setObjectName(QString::fromUtf8("nameEdit_2"));

		formLayout->setWidget(1, QFormLayout::FieldRole, nameEdit_2);

		labelsLabel = new QLabel(groupBox_2);
		labelsLabel->setObjectName(QString::fromUtf8("labelsLabel"));

		formLayout->setWidget(2, QFormLayout::LabelRole, labelsLabel);

		labelsEdit = new QLineEdit(groupBox_2);
		labelsEdit->setObjectName(QString::fromUtf8("labelsEdit"));

		formLayout->setWidget(2, QFormLayout::FieldRole, labelsEdit);

		colorLabel = new QLabel(groupBox_2);
		colorLabel->setObjectName(QString::fromUtf8("colorLabel"));

		formLayout->setWidget(3, QFormLayout::LabelRole, colorLabel);

		colorButton = new QPushButton(groupBox_2);
		colorButton->setObjectName(QString::fromUtf8("colorButton"));

		formLayout->setWidget(3, QFormLayout::FieldRole, colorButton);

		verticalLayout->addLayout(formLayout);

		verticalLayout_2->addWidget(groupBox_2);

		groupBox = new QGroupBox(segObjectInspector);
		groupBox->setObjectName(QString::fromUtf8("groupBox"));
		QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
		groupBox->setSizePolicy(sizePolicy);
		horizontalLayout = new QHBoxLayout(groupBox);
		horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
		notesEdit = new QPlainTextEdit(groupBox);
		notesEdit->setObjectName(QString::fromUtf8("notesEdit"));

		horizontalLayout->addWidget(notesEdit);

		verticalLayout_2->addWidget(groupBox);

		retranslateUi(segObjectInspector);

		QMetaObject::connectSlotsByName(segObjectInspector);
	}			// setupUi

	void retranslateUi(QWidget * segObjectInspector) {
		segObjectInspector->setWindowTitle(QString());
		groupBox_2->
		    setTitle(QApplication::
			     translate("segObjectInspector", "Source Properties", 0, QApplication::UnicodeUTF8));
		nameLabel->
		    setText(QApplication::translate("segObjectInspector", "Name:", 0, QApplication::UnicodeUTF8));
		nameEdit->setText(QString());
		nameLabel_2->
		    setText(QApplication::translate("segObjectInspector", "Segment ID:", 0, QApplication::UnicodeUTF8));
		nameEdit_2->setText(QString());
		labelsLabel->
		    setText(QApplication::translate("segObjectInspector", "Tags:", 0, QApplication::UnicodeUTF8));
		colorLabel->
		    setText(QApplication::translate("segObjectInspector", "Color:", 0, QApplication::UnicodeUTF8));
		colorButton->setText(QString());
		groupBox->
		    setTitle(QApplication::translate("segObjectInspector", "Notes", 0, QApplication::UnicodeUTF8));
		Q_UNUSED(segObjectInspector);
	}			// retranslateUi

};

namespace Ui {
	class segObjectInspector:public Ui_segObjectInspector {
	};
} // namespace Ui QT_END_NAMESPACE
#endif				// UI_SEGOBJECTINSPECTOR_H
