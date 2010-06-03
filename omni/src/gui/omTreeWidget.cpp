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
	QTreeWidget::mousePressEvent(event);
        if (event->button() == Qt::LeftButton) {
		emit leftClicked();
	}
	if (event->button() == Qt::RightButton) {
		emit rightClicked();
	}

	
}

void
OmTreeWidget::keyPressEvent(QKeyEvent* event)
{
	QTreeWidget::keyPressEvent(event);	

	switch (event->key()) {
	case Qt::Key_Up:
		emit keyboardUpArrowPressed();
		break;
	case Qt::Key_Down:
		emit keyboardDownArrowPressed();
		break;
	}
}
