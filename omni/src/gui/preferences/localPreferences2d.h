#ifndef LOCAL_PREFERENCES2D_H
#define LOCAL_PREFERENCES2D_H

#include <QtGui>

class LocalPreferences2d : public QWidget
{ 
    Q_OBJECT 
	
 public: 
	LocalPreferences2d(QWidget *parent = 0); 
    
 private slots: 
	void on_stickyCrosshairMode_stateChanged(int state);

 private:
 	QGroupBox* makeGeneralPropBox();
        QCheckBox  *stickyCrosshairMode;	
}; 
#endif
