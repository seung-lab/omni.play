#ifndef MST_VIEWER_HPP
#define MST_VIEWER_HPP

#include "utility/segmentationDataWrapper.hpp"
#include <QtGui>

class MstViewerImpl : public QTableWidget {
public:
    MstViewerImpl(QWidget * parent, SegmentationDataWrapper sdw);

private:
    SegmentationDataWrapper sdw_;
    void populate();

    template <typename T>
    void setCell(const int rowNum, int& colNum, const T num){
        // auto-incremented column number
        setItem(rowNum, colNum++, makeTableItem(num));
    }

    template <typename T>
    QTableWidgetItem* makeTableItem(const T num){
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setData(0, num); // allows columns to be sorted numerically
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
