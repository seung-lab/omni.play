#include "system/omAppState.hpp"
#include "gui/mainwindow.h"
#include "gui/myInspectorWidget.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "zi/omUtility.h"

#include <QApplication>

QString OmAppState::GetPID() {
	return QString::number(qApp->applicationPid());
}

QString OmAppState::GetHostname() {
	return QString::fromStdString(zi::system::hostname);
}

void OmAppState::SetInspector(MyInspectorWidget* miw){
	instance().inspectorWidget_ = miw;
}

void OmAppState::SetMainWindow(MainWindow* mw){
	instance().mainWindow_ = mw;
}

void OmAppState::SetDendToolBar(DendToolBar* dtb){
	instance().dendToolBar_ = dtb;
}

void OmAppState::UpdateStatusBar(const QString& msg){
	instance().mainWindow_->updateStatusBar(msg);
}

QSize OmAppState::GetViewBoxSizeHint()
{
	QWidget* mw = instance().mainWindow_;
	if(!mw){
		mw = QApplication::activeWindow();
		if(!mw){
			printf("warning: assuming window size is 1000x640\n");
			return QSize(1000, 640);
		}
	}

	int w = mw->width();
	int h = mw->height();

	if(instance().inspectorWidget_){
		w -= instance().inspectorWidget_->width();
	}

	if(instance().dendToolBar_){
		w -= instance().dendToolBar_->width();
	}

	return QSize(w, h);
}
