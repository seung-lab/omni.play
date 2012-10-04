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
#include "gui/widgets/locationEditDialog.hpp"

#include <QtGui>
#include <QDialog>
#include <sstream>

namespace om {
namespace sidebars {

class AnnotationListWidget : public QTreeWidget,
                             public om::events::annotationEventListener,
                             public OmViewEventListener
{
Q_OBJECT

public:
    AnnotationListWidget(QWidget *parent, OmViewGroupState *vgs)
        : QTreeWidget(parent)
        , vgs_(vgs)
    {
        setSelectionMode(QAbstractItemView::SingleSelection);
        setAlternatingRowColors(true);

        QStringList headers;
        headers << tr("") << tr("") << tr("Comment") << tr("Position") << tr("Size");
        setColumnCount(headers.size());
        setColumnWidth(ENABLE_COL, 30);
        setColumnWidth(COLOR_COL, 30);
        setColumnWidth(SIZE_COL, 30);
        setHeaderLabels(headers);

        setFocusPolicy(Qt::StrongFocus);
        populate();

        om::connect(this, SIGNAL(itemSelectionChanged()),
                    this, SLOT(highlightSelected()));
        om::connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
                    this, SLOT(highlightClicked(QTreeWidgetItem *, int)));
        om::connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
                    this, SLOT(doEdit(QTreeWidgetItem *, int)));
        om::connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
                    this, SLOT(itemEdited(QTreeWidgetItem *, int)));
    }

private:
	typedef om::system::ManagedObject<om::annotation::data> managedAnnotation;

public:

    void populate()
    {
        clear();

        FOR_EACH(it, SegmentationDataWrapper::ValidIDs())
        {
            SegmentationDataWrapper sdw(*it);

            om::annotation::manager &annotations = *sdw.GetSegmentation().Annotations();

            FOR_EACH(iter, annotations)
            {
            	om::annotation::data& a = *iter->Object;
            	managedAnnotation& ma = *iter;

                QTreeWidgetItem *row = new QTreeWidgetItem(this);
                row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable |
                              Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);


                Qt::CheckState enabled = iter->Enabled ? Qt::Checked : Qt::Unchecked;
                row->setCheckState(ENABLE_COL, enabled);
                row->setTextAlignment(ENABLE_COL, Qt::AlignCenter);
                row->setIcon(COLOR_COL, om::utils::color::OmColorAsQPixmap(a.color));
                row->setText(TEXT_COL, QString::fromStdString(a.comment));
                setLocationText(row, a);
                row->setText(SIZE_COL, QString::number(a.size));
                row->setData(POSITION_COL, Qt::UserRole, QVariant::fromValue<void *>(&ma));
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
    void CoordSystemChangeEvent() {
       	populate();
    }

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

    void doEdit(QTreeWidgetItem* item, int column)
    {
    	using namespace om::utils;

		managedAnnotation* ann = getAnnotation(item);
		if(!ann) {
			return;
		}
        if (column == COLOR_COL)
        {
        	QColor color = color::OmColorToQColor(ann->Object->color);
        	color = QColorDialog::getColor(color, this);

            if (!color.isValid()) {
                return;
            }

            ann->Object->color = color::QColorToOmColor(color);
            item->setIcon(COLOR_COL, color::OmColorAsQPixmap(ann->Object->color));
        }

        if (column == POSITION_COL)
        {
        	om::globalCoord c = ann->Object->coord.toGlobalCoord();
        	LocationEditDialog::EditLocation(c, this);
        	ann->Object->coord = c.toDataCoord(ann->Object->coord.volume(), 0);
			setLocationText(item, *ann->Object);
        }
    }

    void itemEdited(QTreeWidgetItem* item, int column)
    {
        managedAnnotation* ann = getAnnotation(item);
        if(!ann) {
        	return;
        }

    	if(column == ENABLE_COL) {
			ann->Enabled = item->checkState(ENABLE_COL) == Qt::Checked;
		}

        if(column == TEXT_COL) {
            ann->Object->comment = item->text(TEXT_COL).toStdString();
        }

        if(column == SIZE_COL) {
        	bool success;
            double size = item->text(SIZE_COL).toDouble(&success);
            if(success) {
            	ann->Object->size = size;
            } else {
            	item->setText(SIZE_COL, QString::number(ann->Object->size));
            }
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
                managedAnnotation* selected = getAnnotation(selectedItem);
                if(!selected) {
                	return;
                }
                om::annotation::manager* manager = getManager(selectedItem);
                if(!manager) {
                	return;
                }
                manager->Remove(selected->ID);
                delete selectedItem;
                OmEvents::Redraw2d();
                OmEvents::Redraw3d();
            }
        }
    }

private:
    void highlight(QTreeWidgetItem* item)
    {
        managedAnnotation* annotation = getAnnotation(item);
        if(!annotation) {
        	return;
        }

        vgs_->View2dState()->SetScaledSliceDepth(annotation->Object->coord.toGlobalCoord());
        OmEvents::ViewCenterChanged();
        OmEvents::View3dRecenter();
    }

    static managedAnnotation* getAnnotation(QTreeWidgetItem* item)
    {
    	QVariant data = item->data(POSITION_COL, Qt::UserRole);
		void* ptr = data.value<void *>();
    	return (managedAnnotation*)ptr;
    }

    static om::annotation::manager* getManager(QTreeWidgetItem* item)
    {
        return static_cast<om::annotation::manager*>(
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

    void setLocationText(QTreeWidgetItem* row, const om::annotation::data& a)
    {
    	globalCoord c = a.coord.toGlobalCoord();
    	std::stringstream ss;
    	ss << c.x << ", "
    	   << c.y << ", "
    	   << c.z;
    	row->setText(POSITION_COL, QString::fromStdString(ss.str()));
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