#ifndef LOCAL_PREFERENCES_H
#define LOCAL_PREFERENCES_H

#include <QtGui>

class LocalPreferences : public QWidget
{ 
    Q_OBJECT 
	
public: 
	LocalPreferences(QWidget *parent = 0); 
    
 private slots: 
	void on_numThreads_Slider_valueChanged();

 private:
	QGroupBox* makeNumberOfThreadsBox();
	QSlider *numThreadsSlider;
        QLabel  *numThreadsSliderLabel;	

	QGroupBox* makeBoxGeneric( QLabel** label, QSlider** slider, QString title );
}; 
#endif
