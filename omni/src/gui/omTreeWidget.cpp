#include "omTreeWidget.h"
#include "common/omCommon.h"

OmTreeWidget::OmTreeWidget(QWidget * parent) : QTreeWidget(parent)
{

}

OmTreeWidget::~OmTreeWidget()
{

}

void
OmTreeWidget::mousePressEvent(QMouseEvent* event)
{
        if (event->button() == Qt::LeftButton) {
		emit leftClicked();
	}
	if (event->button() == Qt::RightButton) {
		emit rightClicked();
	}
	QTreeWidget::mousePressEvent(event);
	
}

void
OmTreeWidget::keyPressEvent(QKeyEvent* event)
{
	QTreeWidget::keyPressEvent(event);	
}
