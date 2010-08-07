#ifndef MST_VIEWER_HPP
#define MST_VIEWER_HPP

#include <QtGui>
#include "utility/dataWrappers.h"

class MstViewerImpl : public QTableWidget {
public:
	MstViewerImpl(QWidget * parent, SegmentationDataWrapper sdw);

private:
	SegmentationDataWrapper sdw_;
	void populate();

	template <typename T>
	void add(const int rowNum, int& volNum, const T num){
		setItem(rowNum, volNum++, fromNum(num));
	}

	template <typename T>
	QTableWidgetItem* fromNum(const T num){
		QTableWidgetItem* item = new QTableWidgetItem();
		item->setData(0, num);
		return item;
	}
};

class MstViewer : public QDialog {
public:
	MstViewer(QWidget * p, SegmentationDataWrapper sdw)
		: QDialog(p)
	{
		MstViewerImpl* v = new MstViewerImpl(p, sdw);

		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->addWidget(v);

		setWindowTitle(tr("MST Viewer"));
		resize(500, 400);

		show();
 	}
};



#endif
