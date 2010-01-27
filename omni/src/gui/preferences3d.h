#ifndef PREFERENCES3D_H
#define PREFERENCES3D_H

#include <QWidget> 
#include "ui_preferences3d.h" 

class Preferences3d : public QWidget, public Ui::preferences3d
{ 
    Q_OBJECT 
	
public: 
    Preferences3d(QWidget *parent = 0); 
	
	
	private slots: 
	void on_antiAliasingCheckBox_stateChanged();
	void on_bgColorPicker_clicked();
	void on_highlightCheckBox_stateChanged();
	void on_highlightColorPicker_clicked();
	void on_transparentCheckBox_stateChanged();
	
	void on_fovSlider_valueChanged();
	void on_nearplaneSlider_valueChanged();
	void on_farplaneSlider_valueChanged();
	
	void on_axisCheckBox_stateChanged();
	void on_chunksCheckBox_stateChanged();
	void on_infoCheckBox_stateChanged();
	void on_focusCheckBox_stateChanged();
	
	void on_styleComboBox_currentIndexChanged();
	
	void alphaSliderValueChanged(int val);
}; 
#endif