/********************************************************************************
** Form generated from reading ui file 'preferences3d.ui'
**
** Created: Mon Apr 20 19:40:52 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PREFERENCES3D_H
#define UI_PREFERENCES3D_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE class Ui_preferences3d {
 public:
	QGridLayout * gridLayout_2;
	QGroupBox *groupBox_2;
	QGridLayout *gridLayout_3;
	QLabel *fovLabel;
	QLabel *fovSliderLabel;
	QSlider *fovSlider;
	QLabel *nearplaneLabel;
	QLabel *nearplaneSliderLabel;
	QSlider *nearplaneSlider;
	QLabel *farplaneLabel;
	QLabel *farplaneSliderLabel;
	QSlider *farplaneSlider;
	QGroupBox *groupBox_3;
	QGridLayout *gridLayout_4;
	QHBoxLayout *horizontalLayout;
	QLabel *styleLabel;
	QComboBox *styleComboBox;
	QSpacerItem *horizontalSpacer;
	QCheckBox *infoCheckBox;
	QCheckBox *axisCheckBox;
	QCheckBox *chunksCheckBox;
	QCheckBox *focusCheckBox;
	QGroupBox *groupBox;
	QGridLayout *gridLayout;
	QCheckBox *antiAliasingCheckBox;
	QHBoxLayout *horizontalLayout_4;
	QLabel *bgColorLabel;
	QPushButton *bgColorPicker;
	QSpacerItem *horizontalSpacer_4;
	QCheckBox *highlightCheckBox;
	QHBoxLayout *horizontalLayout_2;
	QSpacerItem *horizontalSpacer_3;
	QLabel *colorLabel;
	QPushButton *highlightColorPicker;
	QSpacerItem *horizontalSpacer_2;
	QCheckBox *transparentCheckBox;
	QHBoxLayout *horizontalLayout_3;
	QLabel *alphaLabel;
	QLabel *transparencySliderLabel;
	QSlider *transparencySlider;

	void setupUi(QWidget * preferences3d) {
		if (preferences3d->objectName().isEmpty())
			preferences3d->setObjectName(QString::fromUtf8("preferences3d"));
		preferences3d->resize(554, 729);
		gridLayout_2 = new QGridLayout(preferences3d);
		gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
		groupBox_2 = new QGroupBox(preferences3d);
		groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
		gridLayout_3 = new QGridLayout(groupBox_2);
		gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
		fovLabel = new QLabel(groupBox_2);
		fovLabel->setObjectName(QString::fromUtf8("fovLabel"));

		gridLayout_3->addWidget(fovLabel, 0, 0, 1, 1);

		fovSliderLabel = new QLabel(groupBox_2);
		fovSliderLabel->setObjectName(QString::fromUtf8("fovSliderLabel"));
		fovSliderLabel->setMinimumSize(QSize(35, 0));

		gridLayout_3->addWidget(fovSliderLabel, 0, 1, 1, 1);

		fovSlider = new QSlider(groupBox_2);
		fovSlider->setObjectName(QString::fromUtf8("fovSlider"));
		fovSlider->setMinimum(30);
		fovSlider->setMaximum(150);
		fovSlider->setOrientation(Qt::Horizontal);
		fovSlider->setTickPosition(QSlider::TicksBelow);
		fovSlider->setTickInterval(10);

		gridLayout_3->addWidget(fovSlider, 0, 2, 1, 1);

		nearplaneLabel = new QLabel(groupBox_2);
		nearplaneLabel->setObjectName(QString::fromUtf8("nearplaneLabel"));

		gridLayout_3->addWidget(nearplaneLabel, 1, 0, 1, 1);

		nearplaneSliderLabel = new QLabel(groupBox_2);
		nearplaneSliderLabel->setObjectName(QString::fromUtf8("nearplaneSliderLabel"));
		nearplaneSliderLabel->setMinimumSize(QSize(35, 0));

		gridLayout_3->addWidget(nearplaneSliderLabel, 1, 1, 1, 1);

		nearplaneSlider = new QSlider(groupBox_2);
		nearplaneSlider->setObjectName(QString::fromUtf8("nearplaneSlider"));
		nearplaneSlider->setMinimum(1);
		nearplaneSlider->setMaximum(1000);
		nearplaneSlider->setOrientation(Qt::Horizontal);
		nearplaneSlider->setTickPosition(QSlider::TicksBelow);
		nearplaneSlider->setTickInterval(100);

		gridLayout_3->addWidget(nearplaneSlider, 1, 2, 1, 1);

		farplaneLabel = new QLabel(groupBox_2);
		farplaneLabel->setObjectName(QString::fromUtf8("farplaneLabel"));

		gridLayout_3->addWidget(farplaneLabel, 2, 0, 1, 1);

		farplaneSliderLabel = new QLabel(groupBox_2);
		farplaneSliderLabel->setObjectName(QString::fromUtf8("farplaneSliderLabel"));
		farplaneSliderLabel->setMinimumSize(QSize(35, 0));

		gridLayout_3->addWidget(farplaneSliderLabel, 2, 1, 1, 1);

		farplaneSlider = new QSlider(groupBox_2);
		farplaneSlider->setObjectName(QString::fromUtf8("farplaneSlider"));
		farplaneSlider->setMinimum(1);
		farplaneSlider->setMaximum(1000);
		farplaneSlider->setOrientation(Qt::Horizontal);
		farplaneSlider->setTickPosition(QSlider::TicksBelow);
		farplaneSlider->setTickInterval(100);

		gridLayout_3->addWidget(farplaneSlider, 2, 2, 1, 1);

		gridLayout_2->addWidget(groupBox_2, 1, 0, 1, 1);

		groupBox_3 = new QGroupBox(preferences3d);
		groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
		gridLayout_4 = new QGridLayout(groupBox_3);
		gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
		horizontalLayout = new QHBoxLayout();
		horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
		styleLabel = new QLabel(groupBox_3);
		styleLabel->setObjectName(QString::fromUtf8("styleLabel"));
		QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(styleLabel->sizePolicy().hasHeightForWidth());
		styleLabel->setSizePolicy(sizePolicy);

		horizontalLayout->addWidget(styleLabel);

		styleComboBox = new QComboBox(groupBox_3);
		styleComboBox->setObjectName(QString::fromUtf8("styleComboBox"));

		horizontalLayout->addWidget(styleComboBox);

		horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout->addItem(horizontalSpacer);

		gridLayout_4->addLayout(horizontalLayout, 4, 0, 1, 1);

		infoCheckBox = new QCheckBox(groupBox_3);
		infoCheckBox->setObjectName(QString::fromUtf8("infoCheckBox"));

		gridLayout_4->addWidget(infoCheckBox, 2, 0, 1, 1);

		axisCheckBox = new QCheckBox(groupBox_3);
		axisCheckBox->setObjectName(QString::fromUtf8("axisCheckBox"));

		gridLayout_4->addWidget(axisCheckBox, 0, 0, 1, 1);

		chunksCheckBox = new QCheckBox(groupBox_3);
		chunksCheckBox->setObjectName(QString::fromUtf8("chunksCheckBox"));

		gridLayout_4->addWidget(chunksCheckBox, 1, 0, 1, 1);

		focusCheckBox = new QCheckBox(groupBox_3);
		focusCheckBox->setObjectName(QString::fromUtf8("focusCheckBox"));

		gridLayout_4->addWidget(focusCheckBox, 3, 0, 1, 1);

		gridLayout_2->addWidget(groupBox_3, 2, 0, 1, 1);

		groupBox = new QGroupBox(preferences3d);
		groupBox->setObjectName(QString::fromUtf8("groupBox"));
		gridLayout = new QGridLayout(groupBox);
		gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
		antiAliasingCheckBox = new QCheckBox(groupBox);
		antiAliasingCheckBox->setObjectName(QString::fromUtf8("antiAliasingCheckBox"));

		gridLayout->addWidget(antiAliasingCheckBox, 0, 0, 1, 1);

		horizontalLayout_4 = new QHBoxLayout();
		horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
		bgColorLabel = new QLabel(groupBox);
		bgColorLabel->setObjectName(QString::fromUtf8("bgColorLabel"));

		horizontalLayout_4->addWidget(bgColorLabel);

		bgColorPicker = new QPushButton(groupBox);
		bgColorPicker->setObjectName(QString::fromUtf8("bgColorPicker"));
		QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
		sizePolicy1.setHorizontalStretch(0);
		sizePolicy1.setVerticalStretch(0);
		sizePolicy1.setHeightForWidth(bgColorPicker->sizePolicy().hasHeightForWidth());
		bgColorPicker->setSizePolicy(sizePolicy1);

		horizontalLayout_4->addWidget(bgColorPicker);

		horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout_4->addItem(horizontalSpacer_4);

		gridLayout->addLayout(horizontalLayout_4, 1, 0, 1, 1);

		highlightCheckBox = new QCheckBox(groupBox);
		highlightCheckBox->setObjectName(QString::fromUtf8("highlightCheckBox"));

		gridLayout->addWidget(highlightCheckBox, 2, 0, 1, 1);

		horizontalLayout_2 = new QHBoxLayout();
		horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
		horizontalSpacer_3 = new QSpacerItem(30, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

		horizontalLayout_2->addItem(horizontalSpacer_3);

		colorLabel = new QLabel(groupBox);
		colorLabel->setObjectName(QString::fromUtf8("colorLabel"));

		horizontalLayout_2->addWidget(colorLabel);

		highlightColorPicker = new QPushButton(groupBox);
		highlightColorPicker->setObjectName(QString::fromUtf8("highlightColorPicker"));
		sizePolicy1.setHeightForWidth(highlightColorPicker->sizePolicy().hasHeightForWidth());
		highlightColorPicker->setSizePolicy(sizePolicy1);

		horizontalLayout_2->addWidget(highlightColorPicker);

		horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout_2->addItem(horizontalSpacer_2);

		gridLayout->addLayout(horizontalLayout_2, 3, 0, 1, 1);

		transparentCheckBox = new QCheckBox(groupBox);
		transparentCheckBox->setObjectName(QString::fromUtf8("transparentCheckBox"));

		gridLayout->addWidget(transparentCheckBox, 4, 0, 1, 1);

		horizontalLayout_3 = new QHBoxLayout();
		horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
		alphaLabel = new QLabel(groupBox);
		alphaLabel->setObjectName(QString::fromUtf8("alphaLabel"));

		horizontalLayout_3->addWidget(alphaLabel);

		transparencySliderLabel = new QLabel(groupBox);
		transparencySliderLabel->setObjectName(QString::fromUtf8("transparencySliderLabel"));
		transparencySliderLabel->setMinimumSize(QSize(40, 0));
		transparencySliderLabel->setMaximumSize(QSize(25, 16777215));

		horizontalLayout_3->addWidget(transparencySliderLabel);

		transparencySlider = new QSlider(groupBox);
		transparencySlider->setObjectName(QString::fromUtf8("transparencySlider"));
		transparencySlider->setMaximum(50);
		transparencySlider->setOrientation(Qt::Horizontal);
		transparencySlider->setTickPosition(QSlider::TicksBelow);
		transparencySlider->setTickInterval(5);

		horizontalLayout_3->addWidget(transparencySlider);

		gridLayout->addLayout(horizontalLayout_3, 5, 0, 1, 1);

		gridLayout_2->addWidget(groupBox, 0, 0, 1, 1);

		retranslateUi(preferences3d);
		QObject::connect(farplaneSlider, SIGNAL(valueChanged(int)), farplaneSliderLabel, SLOT(setNum(int)));
		QObject::connect(nearplaneSlider, SIGNAL(valueChanged(int)), nearplaneSliderLabel, SLOT(setNum(int)));
		QObject::connect(fovSlider, SIGNAL(valueChanged(int)), fovSliderLabel, SLOT(setNum(int)));

		QMetaObject::connectSlotsByName(preferences3d);
	}			// setupUi

	void retranslateUi(QWidget * preferences3d) {
		preferences3d->setWindowTitle(QString());
		groupBox_2->setTitle(QApplication::translate("preferences3d", "Camera", 0, QApplication::UnicodeUTF8));
		fovLabel->
		    setText(QApplication::translate("preferences3d", "Field of View:", 0, QApplication::UnicodeUTF8));
		fovSliderLabel->setText(QString());
		nearplaneLabel->
		    setText(QApplication::translate("preferences3d", "Near Plane:", 0, QApplication::UnicodeUTF8));
		nearplaneSliderLabel->setText(QString());
		farplaneLabel->
		    setText(QApplication::translate("preferences3d", "Far Plane:", 0, QApplication::UnicodeUTF8));
		farplaneSliderLabel->setText(QString());
		groupBox_3->setTitle(QApplication::translate("preferences3d", "Options", 0, QApplication::UnicodeUTF8));
		styleLabel->setText(QApplication::translate("preferences3d", "Style:", 0, QApplication::UnicodeUTF8));
		styleComboBox->clear();
		styleComboBox->insertItems(0, QStringList()
					   << QApplication::translate("preferences3d", "Crosshairs", 0,
								      QApplication::UnicodeUTF8)
					   << QApplication::translate("preferences3d", "Ball", 0,
								      QApplication::UnicodeUTF8)
		    );
		infoCheckBox->
		    setText(QApplication::translate("preferences3d", "Show Information", 0, QApplication::UnicodeUTF8));
		axisCheckBox->
		    setText(QApplication::translate("preferences3d", "Show Axis", 0, QApplication::UnicodeUTF8));
		chunksCheckBox->
		    setText(QApplication::translate("preferences3d", "Show Chunks", 0, QApplication::UnicodeUTF8));
		focusCheckBox->
		    setText(QApplication::translate("preferences3d", "Show Focus", 0, QApplication::UnicodeUTF8));
		groupBox->setTitle(QApplication::translate("preferences3d", "Drawing", 0, QApplication::UnicodeUTF8));
		antiAliasingCheckBox->
		    setText(QApplication::translate("preferences3d", "Anti-aliasing", 0, QApplication::UnicodeUTF8));
		bgColorLabel->
		    setText(QApplication::
			    translate("preferences3d", "Background Color:", 0, QApplication::UnicodeUTF8));
		bgColorPicker->setText(QString());
		highlightCheckBox->
		    setText(QApplication::
			    translate("preferences3d", "Highlight Selected", 0, QApplication::UnicodeUTF8));
		colorLabel->setText(QApplication::translate("preferences3d", "Color:", 0, QApplication::UnicodeUTF8));
		highlightColorPicker->setText(QString());
		transparentCheckBox->
		    setText(QApplication::
			    translate("preferences3d", "Transparent Unselected Segments", 0,
				      QApplication::UnicodeUTF8));
		alphaLabel->setText(QApplication::translate("preferences3d", "Alpha:", 0, QApplication::UnicodeUTF8));
		transparencySliderLabel->
		    setText(QApplication::translate("preferences3d", "0", 0, QApplication::UnicodeUTF8));
		Q_UNUSED(preferences3d);
	}			// retranslateUi

};

namespace Ui {
	class preferences3d:public Ui_preferences3d {
	};
} // namespace Ui QT_END_NAMESPACE
#endif				// UI_PREFERENCES3D_H
