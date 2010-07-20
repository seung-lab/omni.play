#ifndef INSPECTOR_PROPERTIES_H
#define INSPECTOR_PROPERTIES_H

#include <QtGui>
#include <QWidget> 

class OmViewGroupState;

class InspectorProperties : public QDialog
{
	Q_OBJECT

public:
	InspectorProperties(QWidget *parent, OmViewGroupState * vgs);
	void setOrReplaceWidget(QWidget *incomingWidget, const QString title);
	OmViewGroupState * getViewGroupState();


public slots:
	void closeDialog();
	
private:
	QWidget* mWidget;
	QVBoxLayout *mainLayout;
	OmViewGroupState * mViewGroupState;
};

#endif
