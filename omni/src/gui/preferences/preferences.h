#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QtGui>
#include <QWidget> 

class Preferences : public QDialog
{
	Q_OBJECT

public:
	Preferences(QWidget *parent);
	void showProjectPreferences();
	void showLocalPreferences();

private slots:
	void closeDialog();
	
private:
	QTabWidget *tabs;
	QPushButton *closeButton;

};

#endif
