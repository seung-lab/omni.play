#ifndef FILOBJECTINSPECTOR_H
#define FILOBJECTINSPECTOR_H

#include <QWidget> 
#include "ui_filObjectInspector.h" 

class FilObjectInspector : public QWidget, public Ui::filObjectInspector
{ 
    Q_OBJECT 
	
public: 
    FilObjectInspector(QWidget *parent = 0); 
	
	
	private slots: 

}; 
#endif
