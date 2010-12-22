#ifndef INSPECTOR_PROPERTIES_H
#define INSPECTOR_PROPERTIES_H

#include <QtGui>
#include <QWidget>

class OmViewGroupState;

class InspectorProperties : public QDialog{
Q_OBJECT

public:
	InspectorProperties(QWidget *parent, OmViewGroupState * vgs);
	void setOrReplaceWidget(QWidget *incomingWidget, const QString title);
	OmViewGroupState * getViewGroupState();

	bool WidgetIsViewable() const {
		return isVisible();
	}

public slots:
	void closeDialog();

private:
	OmViewGroupState * mViewGroupState;
	QWidget* mWidget;
	QVBoxLayout *mainLayout;
};

#endif
