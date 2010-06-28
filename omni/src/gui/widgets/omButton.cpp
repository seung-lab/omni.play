#include "gui/widgets/omButton.h"
#include "gui/mainwindow.h"

OmButton::OmButton( MainWindow * mw, 
		    const QString & title, 
		    const QString & statusTip,
		    const bool isCheckable )
	: QPushButton(mw)
	, mMainWindow(mw)
{
	setText(title);
	setStatusTip(statusTip);
	setCheckable(isCheckable);
}

void OmButton::mousePressEvent(QMouseEvent* event)
{
	QPushButton::mousePressEvent(event);

        if (event->button() == Qt::LeftButton) {
		doAction();
	}
}
