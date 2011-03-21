#include "system/omConnect.hpp"
#include "preferences3d.h"
#include "events/omEvents.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "common/omDebug.h"

#include <QtGui>

Preferences3d::Preferences3d(QWidget * parent)
    : QWidget(parent)
{
    setupUi(this);

    antiAliasingCheckBox->setChecked(OmPreferences::GetBoolean(om::PREF_VIEW3D_ANTIALIASING_BOOL));
    highlightCheckBox->setChecked(OmPreferences::GetBoolean(om::PREF_VIEW3D_HIGHLIGHT_ENABLED_BOOL));
    transparentCheckBox->setChecked(OmPreferences::GetBoolean(om::PREF_VIEW3D_TRANSPARENT_UNSELECTED_BOOL));

    transparencySlider->setValue(floor(OmPreferences::GetFloat(om::PREF_VIEW3D_TRANSPARENT_ALPHA_FLT) * 50));
    transparencySliderLabel->setNum(OmPreferences::GetFloat(om::PREF_VIEW3D_TRANSPARENT_ALPHA_FLT));

    const Vector3 < float >&color = OmPreferences::GetVector3f(om::PREF_VIEW3D_BACKGROUND_COLOR_V3F);
    QPixmap *pixm = new QPixmap(40, 30);
    QColor newcolor = qRgb(color.x * 255, color.y * 255, color.z * 255);
    pixm->fill(newcolor);
    bgColorPicker->setIcon(QIcon(*pixm));

    const Vector3 < float >&color2 = OmPreferences::GetVector3f(om::PREF_VIEW3D_HIGHLIGHT_COLOR_V3F);
    QPixmap *pixm2 = new QPixmap(40, 30);
    QColor newcolor2 = qRgb(color2.x * 255, color2.y * 255, color2.z * 255);
    pixm2->fill(newcolor2);
    highlightColorPicker->setIcon(QIcon(*pixm2));

    fovSlider->setValue(floor(OmPreferences::GetFloat(om::PREF_VIEW3D_CAMERA_FOV_FLT)));
    fovSliderLabel->setNum(floor(OmPreferences::GetFloat(om::PREF_VIEW3D_CAMERA_FOV_FLT)));

    nearplaneSlider->setValue(floor(OmPreferences::GetFloat(om::PREF_VIEW3D_CAMERA_NEAR_PLANE_FLT)));
    nearplaneSliderLabel->setNum(floor(OmPreferences::GetFloat(om::PREF_VIEW3D_CAMERA_NEAR_PLANE_FLT)));

    farplaneSlider->setValue(floor(OmPreferences::GetFloat(om::PREF_VIEW3D_CAMERA_FAR_PLANE_FLT)));
    farplaneSliderLabel->setNum(floor(OmPreferences::GetFloat(om::PREF_VIEW3D_CAMERA_FAR_PLANE_FLT)));

    axisCheckBox->setChecked(OmPreferences::GetBoolean(om::PREF_VIEW3D_SHOW_AXIS_BOOL));
    chunksCheckBox->setChecked(OmPreferences::GetBoolean(om::PREF_VIEW3D_SHOW_CHUNK_EXTENT_BOOL));
    infoCheckBox->setChecked(OmPreferences::GetBoolean(om::PREF_VIEW3D_SHOW_INFO_BOOL));
    focusCheckBox->setChecked(OmPreferences::GetBoolean(om::PREF_VIEW3D_SHOW_FOCUS_BOOL));

    styleComboBox->setCurrentIndex(OmPreferences::GetInteger(om::PREF_VIEW3D_FOCUS_STYLE_INT));

    om::connect(transparencySlider, SIGNAL(valueChanged(int)), this, SLOT(alphaSliderValueChanged(int)));
}

void Preferences3d::on_antiAliasingCheckBox_stateChanged()
{
    OmPreferences::SetBoolean(om::PREF_VIEW3D_ANTIALIASING_BOOL,
                              antiAliasingCheckBox->isChecked());
}

void Preferences3d::on_bgColorPicker_clicked()
{
    const QColor color = QColorDialog::getColor(Qt::white, this);

    if (color.isValid())
    {
        QPixmap *pixm = new QPixmap(40, 30);
        pixm->fill(color);

        bgColorPicker->setIcon(QIcon(*pixm));

        bgColorPicker->update();

        Vector3 < float >color3f(color.redF(), color.greenF(), color.blueF());
        OmPreferences::SetVector3f(om::PREF_VIEW3D_BACKGROUND_COLOR_V3F, color3f);
    }
}

void Preferences3d::on_highlightCheckBox_stateChanged()
{
    OmPreferences::SetBoolean(om::PREF_VIEW3D_HIGHLIGHT_ENABLED_BOOL,
                              highlightCheckBox->isChecked());
}

void Preferences3d::on_highlightColorPicker_clicked()
{
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        QPixmap *pixm = new QPixmap(40, 30);
        pixm->fill(color);

        highlightColorPicker->setIcon(QIcon(*pixm));

        highlightColorPicker->update();

        Vector3 < float >color3f(color.redF(), color.greenF(), color.blueF());
        OmPreferences::SetVector3f(om::PREF_VIEW3D_HIGHLIGHT_COLOR_V3F, color3f);
        }
}

void Preferences3d::on_transparentCheckBox_stateChanged()
{
    OmPreferences::SetBoolean(om::PREF_VIEW3D_TRANSPARENT_UNSELECTED_BOOL, transparentCheckBox->isChecked());
}

void Preferences3d::on_fovSlider_valueChanged()
{
    OmPreferences::SetFloat(om::PREF_VIEW3D_CAMERA_FOV_FLT, (fovSlider->value() * 1.0));
}

void Preferences3d::on_nearplaneSlider_valueChanged()
{
    OmPreferences::SetFloat(om::PREF_VIEW3D_CAMERA_NEAR_PLANE_FLT, (nearplaneSlider->value()));
}

void Preferences3d::on_farplaneSlider_valueChanged()
{
    OmPreferences::SetFloat(om::PREF_VIEW3D_CAMERA_FAR_PLANE_FLT, (farplaneSlider->value() * 1000.0));
}

void Preferences3d::on_axisCheckBox_stateChanged()
{
    OmPreferences::SetBoolean(om::PREF_VIEW3D_SHOW_AXIS_BOOL, axisCheckBox->isChecked());
}

void Preferences3d::on_chunksCheckBox_stateChanged()
{
    OmPreferences::SetBoolean(om::PREF_VIEW3D_SHOW_CHUNK_EXTENT_BOOL, chunksCheckBox->isChecked());
}

void Preferences3d::on_infoCheckBox_stateChanged()
{
    OmPreferences::SetBoolean(om::PREF_VIEW3D_SHOW_INFO_BOOL, infoCheckBox->isChecked());
}

void Preferences3d::on_focusCheckBox_stateChanged()
{
    OmPreferences::SetBoolean(om::PREF_VIEW3D_SHOW_FOCUS_BOOL, focusCheckBox->isChecked());
}

void Preferences3d::on_styleComboBox_currentIndexChanged()
{
    OmPreferences::SetInteger(om::PREF_VIEW3D_FOCUS_STYLE_INT, styleComboBox->currentIndex());
}

void Preferences3d::alphaSliderValueChanged(int val)
{
    ////debug(genone,"slider value changed = %i\n", val);
    transparencySliderLabel->setNum(val / (50.0));

    OmPreferences::SetFloat(om::PREF_VIEW3D_TRANSPARENT_ALPHA_FLT, (val / (50.0)));
}
