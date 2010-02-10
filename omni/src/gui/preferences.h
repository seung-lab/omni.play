#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QtGui>
#include <QWidget> 

class Preferences : public QDialog
{
	Q_OBJECT

public:
	Preferences(QWidget *parent = 0);

private:
	QTabWidget *tabs;
	QDialogButtonBox *closeButton;

};

#endif
