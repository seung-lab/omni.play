#include <QtGui> 
#include "preferences2d.h" 

#include "common/omStd.h"
#include "volume/omVolume.h"
#include "volume/omVolumeTypes.h"
#include "system/omSystemTypes.h"

#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#define DEBUG 0;

Preferences2d::Preferences2d(QWidget *parent) 
: QWidget(parent) 
{ 
    setupUi(this);
	
	volEdit->blockSignals(true);
	tileEdit->blockSignals(true);
	depthEdit->blockSignals(true);
	sideEdit->blockSignals(true);
	mipEdit->blockSignals(true);
	transparencySlider->blockSignals(true);
	infoCheckBox->blockSignals(true);
	
	volEdit->setText(QString::number(OmPreferences::GetInteger(OM_PREF_VIEW2D_VOLUME_CACHE_SIZE_INT)));
	cout << "after set 'vol cache size' edit" << endl;

	tileEdit->setText(QString::number(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT)));
	cout << "after set 'tile cache size' edit" << endl;

	depthEdit->setText(QString::number(OmPreferences::GetInteger(OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT)));
	cout << "after set 'depth cache size' edit" << endl;

	sideEdit->setText(QString::number(OmPreferences::GetInteger(OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT)));
	mipEdit->setText(QString::number(OmPreferences::GetInteger(OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT)));
					  
	transparencySlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT) * 50));
	transparencySliderLabel->setNum(OmPreferences::GetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT));
	
	infoCheckBox->setChecked(OmPreferences::GetBoolean(OM_PREF_VIEW2D_SHOW_INFO_BOOL));

	volEdit->blockSignals(false);
	tileEdit->blockSignals(false);
	depthEdit->blockSignals(false);
	sideEdit->blockSignals(false);
	mipEdit->blockSignals(false);
	transparencySlider->blockSignals(false);
	infoCheckBox->blockSignals(false);
}

void Preferences2d::on_volEdit_editingFinished() {
	
	OmPreferences::SetInteger(OM_PREF_VIEW2D_VOLUME_CACHE_SIZE_INT, volEdit->text().toInt());

	// OmVolume::Instance()->SetImageCacheMaxSize(volEdit->text().toFloat() * BYTES_PER_MB.0);
	
	// calls preference change event
}

void Preferences2d::on_tileEdit_editingFinished() {
	
	OmPreferences::SetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT, tileEdit->text().toInt());
	
	// calls preference change event
}

void Preferences2d::on_depthEdit_editingFinished() {
	
	OmPreferences::SetInteger(OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT, depthEdit->text().toInt());
}

void Preferences2d::on_sideEdit_editingFinished() {
	
	OmPreferences::SetInteger(OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT, sideEdit->text().toInt());
}

void Preferences2d::on_mipEdit_editingFinished() {
	
	OmPreferences::SetInteger(OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT, mipEdit->text().toInt());

}

void Preferences2d::on_transparencySlider_valueChanged(int val) {
//	DOUT("slider value changed = " << val);
	
	transparencySliderLabel->setNum(val / (50.0));
	
	OmPreferences::SetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT, (val / (50.0)));
	
	// calls preference change event
}

void Preferences2d::on_infoCheckBox_stateChanged() {
	OmPreferences::SetBoolean(OM_PREF_VIEW2D_SHOW_INFO_BOOL, infoCheckBox->isChecked());
	
	//calls preference change event
}
