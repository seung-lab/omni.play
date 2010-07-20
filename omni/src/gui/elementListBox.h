#ifndef ELEMENT_LIST_BOX_H
#define ELEMENT_LIST_BOX_H

#include <QtGui>
#include <QWidget> 

class ElementListBox : public QGroupBox
{
	Q_OBJECT

public:
	ElementListBox( QWidget * );
	void addTab( const int index, QWidget * tab, const QString & tabTitle);
	void reset();
	void setActiveTab( QWidget * tab );

private:
	QTabWidget * mDataElementsTabs;
	QVBoxLayout * mOverallContainer;
	int mCurrentlyActiveTab;
};

#endif
