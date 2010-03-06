#ifndef ELEMENT_LIST_BOX_H
#define ELEMENT_LIST_BOX_H

#include <QtGui>
#include <QWidget> 

class ElementListBox : public QWidget
{
	Q_OBJECT

public:
	ElementListBox( QWidget *, QVBoxLayout * );
	void setBoxTitle( QString title );
	void setTabEnabled( QWidget * tab, QString title );

public slots:

private slots: 

private:
	QTabWidget * dataElementsTabs;
	QGroupBox * groupBox;
	QVBoxLayout * verticalLayout;
	QVBoxLayout* overallContainer;
};

#endif
