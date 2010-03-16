#ifndef LOCAL_PREFERENCES3D_H
#define LOCAL_PREFERENCES3D_H

#include <QtGui>

class LocalPreferences3d : public QWidget
{ 
    Q_OBJECT 
	
 public: 
	LocalPreferences3d(QWidget *parent); 
    
 private slots: 

 private:
 	QGroupBox* makeGeneralPropBox();
}; 
#endif
