#pragma once

#include "common/omCommon.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/color.hpp"
#include "annotation/annotation.h"
#include "events/details/omEventManager.h"
#include "events/details/annotationEvent.h"
#include "viewGroup/omViewGroupView2dState.hpp"

#include <QtGui>
#include <sstream>
#include <system/omConnect.hpp>


Q_DECLARE_METATYPE(DataCoord);

namespace om {
namespace sidebars {
    
class AnnotationListWidget : public QTreeWidget,
                             public om::events::annotationEventListener {
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
        
        OmEventManager::AddListener(OM_ANNOTATION_EVENT_CLASS, this);
        om::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    }
    
    void populate()
    {
        clear();
        
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
                row->setData(POSITION_COL, Qt::UserRole, QVariant::fromValue(a.coord));
            }
        }
    }
    
    void AnnotationModificationEvent(om::events::annotationEvent*) {
        populate();
    }
    
private Q_SLOTS:
    void selectionChanged()
    {
        QList<QTreeWidgetItem*> items = selectedItems();
        if(items.length() > 0) {
            QTreeWidgetItem & selected = *items[0];
            DataCoord coord = selected.data(POSITION_COL, Qt::UserRole).value<DataCoord>();
            
            vgs_->View2dState()->SetScaledSliceDepth(coord);
            OmEvents::ViewCenterChanged();
            OmEvents::View3dRecenter();
        }
    }
    
private:
    OmViewGroupState *vgs_;
    
    static const int COLOR_COL = 0;
    static const int TEXT_COL = 1;
    static const int POSITION_COL = 2;
};

} // namespace sidebars
} // namespace om