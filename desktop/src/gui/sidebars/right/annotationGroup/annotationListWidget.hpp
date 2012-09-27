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
        headers << tr("Enable") << tr("Color") << tr("Comment") << tr("Position");
        setColumnCount(headers.size());
        setColumnWidth(ENABLE_COL, 60);
        setColumnWidth(COLOR_COL, 60);
        setHeaderLabels(headers);

        setFocusPolicy(Qt::StrongFocus);
        populate();

        om::connect(this, SIGNAL(itemSelectionChanged()),
                    this, SLOT(highlightSelected()));
        om::connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
                    this, SLOT(highlightClicked(QTreeWidgetItem *, int)));
        om::connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
                    this, SLOT(itemEdited(QTreeWidgetItem *, int)));
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


                Qt::CheckState enabled = annotations.IsEnabled(a.GetID()) ? Qt::Checked : Qt::Unchecked;
                row->setCheckState(ENABLE_COL, enabled);
                row->setIcon(COLOR_COL, om::utils::color::OmColorAsQPixmap(a.color));
                row->setText(TEXT_COL, QString::fromStdString(a.comment));
                std::stringstream ss;
                ss << a.coord.x << ", "
                   << a.coord.y << ", "
                   << a.coord.z;
                row->setText(POSITION_COL, QString::fromStdString(ss.str()));
                row->setData(POSITION_COL, Qt::UserRole, QVariant::fromValue<void *>(&a));
                row->setData(TEXT_COL, Qt::UserRole, QVariant::fromValue<void *>(&annotations));
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

    void highlightSelected()
    {
        QTreeWidgetItem* selected = getSelected();
        if(selected) {
            highlight(selected);
        }
    }

    void highlightClicked(QTreeWidgetItem* item, int) {
        highlight(item);
    }

    void itemEdited(QTreeWidgetItem* item, int column)
    {
        om::annotation::data& annotation = getAnnotation(item);
        om::annotation::manager& manager = getManager(item);

    	if(column == ENABLE_COL)
		{
			manager.SetEnabled(annotation.GetID(),
				item->checkState(ENABLE_COL) == Qt::Checked);
		}

        if(column == TEXT_COL) {
            annotation.comment = item->text(TEXT_COL).toStdString();
        }

        OmEvents::Redraw2d();
        OmEvents::Redraw3d();
    }
protected:
    void keyReleaseEvent(QKeyEvent * event)
    {
        if(event->key() == Qt::Key_Backspace ||
           event->key() == Qt::Key_Delete)
        {
            QTreeWidgetItem* selectedItem = getSelected();
            if(selectedItem)
            {
                om::annotation::data& selected = getAnnotation(selectedItem);
                om::annotation::manager& manager = getManager(selectedItem);
                manager.Remove(selected.id);
                delete selectedItem;
                OmEvents::Redraw2d();
                OmEvents::Redraw3d();
            }
        }
    }

private:
    void highlight(QTreeWidgetItem* item)
    {
        om::annotation::data& annotation = getAnnotation(item);

        vgs_->View2dState()->SetScaledSliceDepth(annotation.coord);
        OmEvents::ViewCenterChanged();
        OmEvents::View3dRecenter();
    }

    om::annotation::data& getAnnotation(QTreeWidgetItem* item)
    {
        return *static_cast<om::annotation::data*>(
            item->data(POSITION_COL, Qt::UserRole).value<void *>());
    }

    om::annotation::manager& getManager(QTreeWidgetItem* item)
    {
        return *static_cast<om::annotation::manager*>(
        item->data(TEXT_COL, Qt::UserRole).value<void *>());
    }

    QTreeWidgetItem* getSelected()
    {
        QList<QTreeWidgetItem*> items = selectedItems();
        if(items.length() > 0) {
            return items[0];
        }
        return NULL;
    }

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

    OmViewGroupState *vgs_;

    static const int ENABLE_COL = 0;
    static const int COLOR_COL = 1;
    static const int TEXT_COL = 2;
    static const int POSITION_COL = 3;
    static const int SIZE_COL = 4;
};

} // namespace sidebars
} // namespace om