#include "inspectorProperties.h"
#include "system/viewGroup/omViewGroupState.h"

InspectorProperties::InspectorProperties(QWidget *parent, OmViewGroupState * vgs) : QDialog(parent), mViewGroupState(vgs)
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

	if( !isVisible() ){
		show();
		raise();
		activateWindow();
	}
}

void InspectorProperties::closeDialog()
{
	QDialog::done(0);
}

OmViewGroupState * InspectorProperties::getViewGroupState()
{
	return mViewGroupState;
}
