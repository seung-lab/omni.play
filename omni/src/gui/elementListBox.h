#ifndef ELEMENT_LIST_BOX_H
#define ELEMENT_LIST_BOX_H

#include <QtGui>
#include <QWidget> 

class ElementListBox : public QWidget
{
	Q_OBJECT

public:
	ElementListBox( QWidget *, QVBoxLayout * );
	void addTab(int index, QString boxTitle, QWidget * tab, QString tabTitle);
	void clear();

public slots:

private slots: 

private:
	QTabWidget * dataElementsTabs;
	QGroupBox * groupBox;
	QVBoxLayout * verticalLayout;
	QVBoxLayout* overallContainer;
	void setupBox();
};

#endif
