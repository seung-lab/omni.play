#include "preferences2d.h"
#include "common/omStd.h"
#include "system/omEvents.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "common/omDebug.h"

Preferences2d::Preferences2d(QWidget * parent)
	: QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout(this);
	overallContainer->addWidget(makeCacheBox());
	overallContainer->addWidget(makeDisplayBox());
	overallContainer->insertStretch(2, 1);

	set_initial_values();

	QMetaObject::connectSlotsByName(this);
}

QGroupBox* Preferences2d::makeCacheBox()
{
	QGroupBox* groupBox = new QGroupBox("Cache Properties");
	QGridLayout *grid = new QGridLayout( groupBox );

	QLabel *volLabel = new QLabel(groupBox);
	volLabel->setObjectName(QString::fromUtf8("volLabel"));
	volLabel->setText("Volume Cache Size:");
	volLabel->setToolTip("(MB)");
	grid->addWidget(volLabel, 0, 1, 1, 1);

	volEdit = new QLineEdit(groupBox);
	volEdit->setObjectName(QString::fromUtf8("volEdit"));
	grid->addWidget(volEdit, 0, 2, 1, 1);

	QLabel *cacheLabel = new QLabel(groupBox);
	cacheLabel->setObjectName(QString::fromUtf8("cacheLabel"));
	cacheLabel->setText("Tile Cache Size:");
	cacheLabel->setToolTip("(MB)");
	grid->addWidget(cacheLabel, 1, 1, 1, 1);

	tileEdit = new QLineEdit(groupBox);
	tileEdit->setObjectName(QString::fromUtf8("tileEdit"));
	grid->addWidget(tileEdit, 1, 2, 1, 1);

	QLabel *depthLabel = new QLabel(groupBox);
	depthLabel->setObjectName(QString::fromUtf8("depthLabel"));
	depthLabel->setText("Depth Caching:");
	depthLabel->setToolTip("(slices)");
	grid->addWidget(depthLabel, 2, 1, 1, 1);

	depthEdit = new QLineEdit(groupBox);
	depthEdit->setObjectName(QString::fromUtf8("depthEdit"));
	grid->addWidget(depthEdit, 2, 2, 1, 1);

	QLabel *sideLabel = new QLabel(groupBox);
	sideLabel->setObjectName(QString::fromUtf8("sideLabel"));
	sideLabel->setText("Side Caching:");
	sideLabel->setToolTip("(tiles)");
	grid->addWidget(sideLabel, 3, 1, 1, 1);

	sideEdit = new QLineEdit(groupBox);
	sideEdit->setObjectName(QString::fromUtf8("sideEdit"));
	grid->addWidget(sideEdit, 3, 2, 1, 1);

	QLabel *mipLabel = new QLabel(groupBox);
	mipLabel->setObjectName(QString::fromUtf8("mipLabel"));
	mipLabel->setText("Mip Level Caching:");
	grid->addWidget(mipLabel, 4, 1, 1, 1);

	mipEdit = new QLineEdit(groupBox);
	mipEdit->setObjectName(QString::fromUtf8("mipEdit"));
	grid->addWidget(mipEdit, 4, 2, 1, 1);

	return groupBox;
}

QGroupBox* Preferences2d::makeDisplayBox()
{
	QGroupBox* groupBox = new QGroupBox("Cache Properties");
	QGridLayout *grid = new QGridLayout( groupBox );

	QLabel *alphaLabel = new QLabel(groupBox);
	alphaLabel->setObjectName(QString::fromUtf8("alphaLabel"));
	alphaLabel->setText("Alpha:");
	grid->addWidget(alphaLabel, 0, 0);

	transparencySliderLabel = new QLabel(groupBox);
	transparencySliderLabel->setObjectName(QString::fromUtf8("transparencySliderLabel"));
	transparencySliderLabel->setMinimumSize(QSize(40, 0));
	transparencySliderLabel->setMaximumSize(QSize(25, 16777215));
	grid->addWidget(transparencySliderLabel, 0, 1);

	transparencySlider = new QSlider(groupBox);
	transparencySlider->setObjectName(QString::fromUtf8("transparencySlider"));
	transparencySlider->setMaximum(50);
	transparencySlider->setOrientation(Qt::Horizontal);
	transparencySlider->setTickPosition(QSlider::TicksBelow);
	transparencySlider->setTickInterval(5);
	grid->addWidget(transparencySlider, 1, 0);

	infoCheckBox = new QCheckBox(groupBox);
	infoCheckBox->setObjectName(QString::fromUtf8("infoCheckBox"));
	infoCheckBox->setText("Show Information");
	grid->addWidget(infoCheckBox, 2, 0);

	return groupBox;
}

void Preferences2d::set_initial_values()
{
	volEdit->setText(QString::number(OmPreferences::GetInteger(om::PREF_VIEW2D_VOLUME_CACHE_SIZE_INT)));

	tileEdit->setText(QString::number(OmPreferences::GetInteger(om::PREF_VIEW2D_TILE_CACHE_SIZE_INT)));

	depthEdit->setText(QString::number(OmPreferences::GetInteger(om::PREF_VIEW2D_DEPTH_CACHE_SIZE_INT)));

	sideEdit->setText(QString::number(OmPreferences::GetInteger(om::PREF_VIEW2D_SIDES_CACHE_SIZE_INT)));
	mipEdit->setText(QString::number(OmPreferences::GetInteger(om::PREF_VIEW2D_MIP_CACHE_SIZE_INT)));

	transparencySlider->setValue(floor(OmPreferences::GetFloat(om::PREF_VIEW2D_TRANSPARENT_ALPHA_FLT) * 50));
	transparencySliderLabel->setNum(OmPreferences::GetFloat(om::PREF_VIEW2D_TRANSPARENT_ALPHA_FLT));

	infoCheckBox->setChecked(OmPreferences::GetBoolean(om::PREF_VIEW2D_SHOW_INFO_BOOL));
}

void Preferences2d::on_volEdit_editingFinished()
{
	OmPreferences::SetInteger(om::PREF_VIEW2D_VOLUME_CACHE_SIZE_INT, volEdit->text().toInt());
}

void Preferences2d::on_tileEdit_editingFinished()
{
	OmPreferences::SetInteger(om::PREF_VIEW2D_TILE_CACHE_SIZE_INT, tileEdit->text().toInt());
}

void Preferences2d::on_depthEdit_editingFinished()
{
	OmPreferences::SetInteger(om::PREF_VIEW2D_DEPTH_CACHE_SIZE_INT, depthEdit->text().toInt());
}

void Preferences2d::on_sideEdit_editingFinished()
{
	OmPreferences::SetInteger(om::PREF_VIEW2D_SIDES_CACHE_SIZE_INT, sideEdit->text().toInt());
}

void Preferences2d::on_mipEdit_editingFinished()
{
	OmPreferences::SetInteger(om::PREF_VIEW2D_MIP_CACHE_SIZE_INT, mipEdit->text().toInt());
}

void Preferences2d::on_transparencySlider_valueChanged(int val)
{
	transparencySliderLabel->setNum(val / (50.0));
	OmPreferences::SetFloat(om::PREF_VIEW2D_TRANSPARENT_ALPHA_FLT, (val / (50.0)));
}

void Preferences2d::on_infoCheckBox_stateChanged()
{
	OmPreferences::SetBoolean(om::PREF_VIEW2D_SHOW_INFO_BOOL, infoCheckBox->isChecked());
}
