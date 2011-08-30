#pragma once

#include "common/omCommon.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/color.hpp"
#include "annotation/annotation.h"

#include <QtGui>
#include <sstream>


namespace om {
namespace sidebars {
    
class AnnotationListWidget : public QTreeWidget {
Q_OBJECT

public:
    AnnotationListWidget(QWidget *parent, OmViewGroupState *vgs)
        : QTreeWidget(parent)
        , vgs_(vgs)
    {
        setSelectionMode(QAbstractItemView::SingleSelection);
        setAlternatingRowColors(true);
        
        QStringList headers;
        headers << tr("Color") << tr("Comment") << tr("Position");
        setColumnCount(headers.size());
        setColumnWidth(COLOR_COL, 60);
        setHeaderLabels(headers);
        
        setFocusPolicy(Qt::StrongFocus);
        populate();
    }
    
    void populate()
    {
        FOR_EACH(it, SegmentationDataWrapper::ValidIDs())
        {
            SegmentationDataWrapper sdw(*it);
            
            om::annotation::manager &annotations = *sdw.GetSegmentation().Annotations();
            
            FOR_EACH(i, annotations.GetValidIds())
            {
                om::annotation::data& a = annotations.Get(*i);
                
                QTreeWidgetItem *row = new QTreeWidgetItem(this);
                row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable |
                              Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                
                
                row->setIcon(COLOR_COL, om::utils::color::OmColorAsQPixmap(a.color));
                row->setText(TEXT_COL, QString::fromStdString(a.comment));
                std::stringstream ss;
                ss << a.coord.x << ", " << a.coord.y << ", " << a.coord.z;
                row->setText(POSITION_COL, QString::fromStdString(ss.str()));
            }
        }
    }
    
    
private:
    const OmViewGroupState *vgs_;
    
    static const int COLOR_COL = 0;
    static const int TEXT_COL = 1;
    static const int POSITION_COL = 2;
};

} // namespace sidebars
} // namespace om