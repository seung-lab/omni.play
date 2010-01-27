#ifndef VOLUMEINSPECTOR_H
#define VOLUMEINSPECTOR_H

#include <QWidget> 
#include "ui_volumeInspector.h" 

class VolumeInspector : public QWidget, public Ui::volumeInspector
{ 
    Q_OBJECT 
	
public: 
    VolumeInspector(QWidget *parent = 0); 
	
	
	private slots: 
	void on_nameEdit_editingFinished();
	void on_scaleEdit_editingFinished();
	void on_resolutionEdit_editingFinished();
	void on_extentEdit_editingFinished();
	void on_notesEdit_textChanged();
	
	void on_sizeSlider_valueChanged();
	
	void on_ramSlider_valueChanged();
	void on_vramSlider_valueChanged();
}; 
#endif