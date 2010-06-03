#ifndef ELEMENT_LIST_BOX_H
#define ELEMENT_LIST_BOX_H

#include <QtGui>
#include <QWidget> 

class ElementListBox : public QWidget
{
	Q_OBJECT

public:
	ElementListBox( QWidget *, QVBoxLayout * );
	void addTab(QString boxTitle, QWidget * tab, QString tabTitle);
	void clear();

	QPushButton * prevButton;
	QPushButton * nextButton;
	
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
