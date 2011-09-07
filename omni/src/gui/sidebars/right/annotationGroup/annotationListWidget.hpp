#pragma once

#include "common/omCommon.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/color.hpp"
#include "annotation/annotation.h"
#include "events/details/omEventManager.h"
#include "events/details/annotationEvent.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "view2d/omView2dConverters.hpp"
#include "utility/dataWrappers.h"
#include "system/omConnect.hpp"
#include "events/details/omViewEvent.h"

#include <QtGui>
#include <sstream>


Q_DECLARE_METATYPE(DataCoord);

namespace om {
namespace sidebars {
    
class AnnotationListWidget : public QTreeWidget,
                             public om::events::annotationEventListener,
                             public OmViewEventListener {
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
        
        om::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    }
    
    void populate()
    {
        clear();
        
        Vector3i offsets = getOffsets();
        
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
                ss << a.coord.x + offsets.x << ", " 
                   << a.coord.y + offsets.y << ", " 
                   << a.coord.z + offsets.z;
                row->setText(POSITION_COL, QString::fromStdString(ss.str()));
                row->setData(POSITION_COL, Qt::UserRole, QVariant::fromValue(a.coord));
            }
        }
    }
    
    
    
    void AnnotationModificationEvent(om::events::annotationEvent*) {
        populate();
    }
    
    // View Event Listener Implementation
    void ViewBoxChangeEvent() {}
    void ViewCenterChangeEvent() {}
    void ViewPosChangeEvent() {}
    void ViewRedrawEvent() {}
    void ViewBlockingRedrawEvent() {}
    void AbsOffsetChangeEvent() 
    { populate(); }
    
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
    // TODO: remove: hack for abs coords
    OmMipVolume* getVol()
    {
        {
            const ChannelDataWrapper cdw = vgs_->Channel();
            if(cdw.IsValidWrapper()){
                return cdw.GetChannelPtr();
            }
        }
        
        {
            const SegmentationDataWrapper sdw = vgs_->Segmentation();
            if(sdw.IsValidWrapper()){
                return sdw.GetSegmentationPtr();
            }
        }
        
        return NULL;
    }
    
    // TODO: remove: hack for abs coords
    Vector3i getOffsets()
    {
        OmMipVolume* vol = getVol();
        
        if(vol)
        {
            return vol->Coords().GetAbsOffset();
        }
        
        return Vector3i(0,0,0);
    }
    
    OmViewGroupState *vgs_;
    
    static const int COLOR_COL = 0;
    static const int TEXT_COL = 1;
    static const int POSITION_COL = 2;
};

} // namespace sidebars
} // namespace om