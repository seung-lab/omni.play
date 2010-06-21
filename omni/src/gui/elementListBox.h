#ifndef ELEMENT_LIST_BOX_H
#define ELEMENT_LIST_BOX_H

#include <QtGui>
#include <QWidget> 

class ElementListBox : public QGroupBox
{
	Q_OBJECT

public:
	ElementListBox( QWidget * );
	int addTab( const int index, QWidget * tab, const QString & tabTitle);
	void reset();
	void setActiveTab( const int index );

private:
	QTabWidget * dataElementsTabs;
	QVBoxLayout * overallContainer;
	int currentlyActiveTab;
};

#endif
