#ifndef OMTREEWIDGET_H
#define OMTREEWIDGET_H

#include <QtGui>

class OmTreeWidget : public QTreeWidget {
 Q_OBJECT

 public:
	OmTreeWidget(QWidget* parent);
	~OmTreeWidget();

 signals:
	void leftClicked();
	void rightClicked();
	void keyboardUpArrowPressed();
	void  keyboardDownArrowPressed();

 private:
	void mousePressEvent(QMouseEvent * event);
	void keyPressEvent(QKeyEvent * event);

};

#endif
