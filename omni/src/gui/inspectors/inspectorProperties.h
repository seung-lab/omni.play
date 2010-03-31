#ifndef INSPECTOR_PROPERTIES_H
#define INSPECTOR_PROPERTIES_H

#include <QtGui>
#include <QWidget> 

class InspectorProperties : public QDialog
{
	Q_OBJECT

public:
	InspectorProperties(QWidget *parent);
	void setOrReplaceWidget(QWidget *incomingWidget, const QString title);

private slots:
	void closeDialog();
	
private:
	QWidget* mWidget;
	QVBoxLayout *mainLayout;
};

#endif
