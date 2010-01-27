#include <QtGui>
#include "volumeInspector.h"

#include "volume/omVolume.h"
#include "volume/omVolumeTypes.h"
#include "system/omSystemTypes.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include <vmmlib/vmmlib.h>
#include "system/omDebug.h"
using namespace vmml;

VolumeInspector::VolumeInspector(QWidget * parent)
 : QWidget(parent)
{
	setupUi(this);

	const string & my_name = (*OmVolume::Instance()).GetName();
	nameEdit->setText(QString::fromStdString(my_name));
	nameEdit->setMinimumWidth(200);

	const string & my_notes = (*OmVolume::Instance()).GetNote();
	notesEdit->setPlainText(QString::fromStdString(my_notes));

	Vector3 < float >scale = OmVolume::GetScale();
	scaleEdit->setText("[" + QString::number(scale.x) + " " + QString::number(scale.y) + " " +
			   QString::number(scale.z) + "]");

	Vector3 < float >res = OmVolume::GetDataResolution();
	resolutionEdit->setText("[" + QString::number(res.x) + " " + QString::number(res.y) + " " +
				QString::number(res.z) + "]");

	//      const DataBbox& data_extent = OmVolume::GetDataExtent();
	//      Vector3<float> de_min = data_extent.getMin();
	//      Vector3<float> de_max = data_extent.getMax();
	//      extentEdit->setText("[" + QString::number(de_min.x) + " " + QString::number(de_min.y) + " " + QString::number(de_min.z) + "]"
	//                                       " [" + QString::number(de_max.x) + " " + QString::number(de_max.y) + " " + QString::number(de_max.z) + "]");

	// Vector3i OmVolume::GetDataDimensions()
	Vector3i dimen = OmVolume::GetDataDimensions();
	extentEdit->setText("[" + QString::number(dimen.x) + " " + QString::number(dimen.y) + " " +
			    QString::number(dimen.z) + "]");

	int my_chunk_size = OmVolume::GetChunkDimension();
	QString str;
	sizeSlider->setSliderPosition(my_chunk_size / 2);
	sizeLabel->setNum(my_chunk_size);

	ramSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT)));
	ramSizeLabel->setNum(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT)));

	vramSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT)));
	vramSizeLabel->setNum(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT)));
}

void VolumeInspector::on_nameEdit_editingFinished()
{
	OmVolume::Instance()->SetName(nameEdit->text().toStdString());
}

void VolumeInspector::on_scaleEdit_editingFinished()
{
	QString scale = scaleEdit->text();
	scale.chop(1);
	scale = scale.remove(0, 1);

	QString x = scale.section(" ", 0, 0);
	QString y = scale.section(" ", 1, 1);
	QString z = scale.section(" ", 2, 2);

	// bool SetTranslation(const Vector3<float> &vec);
	Vector3 < float >scale_vec = Vector3 < float >(x.toFloat(), y.toFloat(), z.toFloat());
	OmVolume::Instance()->SetScale(scale_vec);
}

void VolumeInspector::on_resolutionEdit_editingFinished()
{
	QString res = resolutionEdit->text();
	res.chop(1);
	res = res.remove(0, 1);

	QString x = res.section(" ", 0, 0);
	QString y = res.section(" ", 1, 1);
	QString z = res.section(" ", 2, 2);

	// bool SetTranslation(const Vector3<float> &vec);
	Vector3 < float >res_vec = Vector3 < float >(x.toFloat(), y.toFloat(), z.toFloat());
	OmVolume::Instance()->SetDataResolution(res_vec);
}

void VolumeInspector::on_extentEdit_editingFinished()
{
	//      extentEdit->setText(QString::number(de_min.x) + " " + QString::number(de_min.y) + " " + QString::number(de_min.z) + "]"
	//                                              " [" + QString::number(de_max.x) + " " + QString::number(de_max.y) + " " + QString::number(de_max.z));

	QString extent = extentEdit->text();
	extent.chop(1);
	extent = extent.remove(0, 1);

	QString min_x = extent.section(" ", 0, 0);
	QString min_y = extent.section(" ", 1, 1);
	QString min_z = extent.section(" ", 2, 2);
	//min_z.chop(1);

	//      //debug("genone","min_x = " << min_x);
	//              //debug("genone","min_y = " << min_x);
	//              //debug("genone","min_z = " << min_x);

	Vector3i data_extent = Vector3i(min_x.toInt(), min_y.toInt(), min_z.toInt());

	OmVolume::Instance()->SetDataDimensions(data_extent);

	//      QErrorMessage *em = new QErrorMessage(this);
	//      em->showMessage("MIN x = " + min_x + " y = " + min_y + " z = " + min_z + "   MAX x = " + max_x + " y = " + max_y + " z = " + max_z);
}

void VolumeInspector::on_notesEdit_textChanged()
{
	OmVolume::Instance()->SetNote(notesEdit->toPlainText().toStdString());

}

void VolumeInspector::on_sizeSlider_valueChanged()
{
	sizeLabel->setNum(sizeSlider->value() * 2);
	OmVolume::SetChunkDimension(sizeSlider->value() * 2);
}

void VolumeInspector::on_ramSlider_valueChanged()
{
	/*
	   ramSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT)));
	   ramSizeLabel->setNum(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT)));
	 */

	OmPreferences::SetFloat(OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT, (ramSlider->value() * 1.0));
	ramSizeLabel->setNum(ramSlider->value());
}

void VolumeInspector::on_vramSlider_valueChanged()
{
	/*
	   vramSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT)));
	   vramSizeLabel->SetNum(floor(OmPreferences::GetFloat(OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT)));
	 */

	OmPreferences::SetFloat(OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT, (vramSlider->value() * 1.0));
	vramSizeLabel->setNum(vramSlider->value());
}
