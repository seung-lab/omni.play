#include "inspectorProperties.h"

InspectorProperties::InspectorProperties(QWidget *parent) : QDialog(parent)
{
	mWidget = NULL;
	
	mainLayout = new QVBoxLayout();
	setLayout(mainLayout);
}

void InspectorProperties::setOrReplaceWidget(QWidget *incomingWidget, const QString title)
{
	if( mWidget != NULL ){
		mainLayout->removeWidget( mWidget );
		mWidget->close();
		delete( mWidget );
		mWidget = NULL;
	}

	mWidget = incomingWidget;
	mainLayout->addWidget( mWidget );
	setWindowTitle(title);

	show();
	raise();
	activateWindow();
}

void InspectorProperties::closeDialog()
{
	QDialog::done(0);
}
