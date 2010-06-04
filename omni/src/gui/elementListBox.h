#ifndef ELEMENT_LIST_BOX_H
#define ELEMENT_LIST_BOX_H

#include <QtGui>
#include <QWidget> 

class ElementListBox : public QGroupBox
{
	Q_OBJECT

public:
	ElementListBox( QWidget *, QVBoxLayout * );
	void addTab(int index, QWidget * tab, QString tabTitle);
	void reset(const QString & boxTitle);

private:
	QTabWidget * dataElementsTabs;
	QVBoxLayout * overallContainer;
};

#endif
