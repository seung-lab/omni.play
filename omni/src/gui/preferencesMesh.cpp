#include <QtGui> 
#include "preferencesMesh.h" 

#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omDebug.h"


PreferencesMesh::PreferencesMesh(QWidget *parent) 
: QWidget(parent) 
{ 
    setupUi(this); 
	
	tdpSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_MESH_REDUCTION_PERCENT_FLT)));
	tdpSliderLabel->setNum(tdpSlider->value());
	pasSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT)));
	pasSliderLabel->setNum(pasSlider->value());
	nsSlider->setValue(OmPreferences::GetInteger(OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT));
	nsSliderLabel->setNum(nsSlider->value());


}

void PreferencesMesh::on_tdpSlider_valueChanged()
{
	OmPreferences::SetFloat(OM_PREF_MESH_REDUCTION_PERCENT_FLT, tdpSlider->value());

	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::UPDATE_PREFERENCES));

}

void PreferencesMesh::on_pasSlider_valueChanged()
{
	OmPreferences::SetFloat(OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT, pasSlider->value());
	
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::UPDATE_PREFERENCES));
}

void PreferencesMesh::on_nsSlider_valueChanged()
{
	OmPreferences::SetInteger(OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT, nsSlider->value());
	
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::UPDATE_PREFERENCES));
}