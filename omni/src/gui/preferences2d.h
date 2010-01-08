#ifndef PREFERENCES2D_H
#define PREFERENCES2D_H

#include <QWidget> 
#include "ui_preferences2d.h" 

class Preferences2d : public QWidget, public Ui::preferences2d
{ 
    Q_OBJECT 
	
public: 
    Preferences2d(QWidget *parent = 0); 
	
	
	private slots: 
	void on_volEdit_editingFinished();
	void on_tileEdit_editingFinished();
	void on_depthEdit_editingFinished();
	void on_sideEdit_editingFinished();
	void on_mipEdit_editingFinished();
	void on_transparencySlider_valueChanged(int val);
	void on_infoCheckBox_stateChanged();

}; 
#endif