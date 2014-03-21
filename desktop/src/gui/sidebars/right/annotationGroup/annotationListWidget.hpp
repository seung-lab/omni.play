#pragma once
#include "precomp.h"

#include "annotation/annotation.h"
#include "common/common.h"
#include "events/listeners.h"
#include "gui/widgets/locationEditDialog.hpp"
#include "system/omConnect.hpp"
#include "utility/color.hpp"
#include "view2d/omView2dConverters.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/channelDataWrapper.hpp"
#include "volume/omSegmentation.h"

namespace om {
namespace sidebars {

class AnnotationListWidget : public QTreeWidget,
                             public om::event::AnnotationEventListener,
                             public om::event::View2dEventListener {
  Q_OBJECT;

 public:
  AnnotationListWidget(QWidget* parent, OmViewGroupState& vgs)
      : QTreeWidget(parent), vgs_(vgs), editing_(false) {
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAlternatingRowColors(true);

    QStringList headers;
    headers << tr("") << tr("") << tr("Comment") << tr("Position")
            << tr("Size");
    setColumnCount(headers.size());
    setColumnWidth(ENABLE_COL, 30);
    setColumnWidth(COLOR_COL, 30);
    setColumnWidth(SIZE_COL, 30);
    setHeaderLabels(headers);

    setFocusPolicy(Qt::StrongFocus);
    populate();
    om::connect(this, SIGNAL(itemSelectionChanged()), this,
                SLOT(highlightSelected()));
    om::connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this,
                SLOT(highlightClicked(QTreeWidgetItem*, int)));
    om::connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this,
                SLOT(doEdit(QTreeWidgetItem*, int)));
    om::connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this,
                SLOT(itemEdited(QTreeWidgetItem*, int)));
  }

 private:
  typedef om::system::ManagedObject<om::annotation::data> managedAnnotation;

 public:
  void populate() {
    clear();

    for (auto& seg : SegmentationDataWrapper::GetPtrVec()) {
      auto& annotations = seg->Annotations();

      FOR_EACH(iter, annotations) {
        om::annotation::data& a = *iter->Object;
        managedAnnotation& ma = *iter;

        QTreeWidgetItem* row = new QTreeWidgetItem(this);
        row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable |
                      Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

        Qt::CheckState enabled = iter->Enabled ? Qt::Checked : Qt::Unchecked;
        row->setCheckState(ENABLE_COL, enabled);
        row->setTextAlignment(ENABLE_COL, Qt::AlignCenter);
        row->setIcon(COLOR_COL, om::utils::color::ColorAsQPixmap(a.color));
        row->setText(TEXT_COL, QString::fromStdString(a.comment));
        setLocationText(row, a);
        row->setText(SIZE_COL, QString::number(a.size));
        row->setData(POSITION_COL, Qt::UserRole,
                     QVariant::fromValue<void*>(&ma));
        row->setData(TEXT_COL, Qt::UserRole,
                     QVariant::fromValue<void*>(&annotations));
      }
    }
  }

  void AnnotationModificationEvent(om::event::AnnotationEvent*) { populate(); }

  // View Event Listener Implementation
  void ViewBoxChangeEvent() {}
  void ViewCenterChangeEvent() {}
  void ViewPosChangeEvent() {}
  void ViewRedrawEvent() {}
  void ViewBlockingRedrawEvent() {}
  void CoordSystemChangeEvent() { populate(); }

 private
Q_SLOTS:

  void highlightSelected() {
    QTreeWidgetItem* selected = getSelected();
    if (selected) {
      highlight(selected);
    }
  }

  void highlightClicked(QTreeWidgetItem* item, int) { highlight(item); }

  void doEdit(QTreeWidgetItem* item, int column) {
    using namespace om::utils;

    managedAnnotation* ann = getAnnotation(item);
    if (!ann) {
      return;
    }
    if (column == COLOR_COL) {
      QColor color = om::utils::color::ColorToQColor(ann->Object->color);
      color = QColorDialog::getColor(color, this);

      if (!color.isValid()) {
        return;
      }

      ann->Object->color = om::utils::color::QColorToColor(color);
      item->setIcon(COLOR_COL,
                    om::utils::color::ColorAsQPixmap(ann->Object->color));
    }

    if (column == POSITION_COL) {
      om::coords::Global c = ann->Object->coord.ToGlobal();
      LocationEditDialog::EditLocation(c, this);
      ann->Object->coord = c.ToData(ann->Object->coord.volume(), 0);
      setLocationText(item, *ann->Object);
    }

    if (column == TEXT_COL) {
      editing_ = true;
    }
  }

  void itemEdited(QTreeWidgetItem* item, int column) {
    managedAnnotation* ann = getAnnotation(item);
    if (!ann) {
      return;
    }

    if (column == ENABLE_COL) {
      ann->Enabled = item->checkState(ENABLE_COL) == Qt::Checked;
    }

    if (column == TEXT_COL) {
      ann->Object->comment = item->text(TEXT_COL).toStdString();
      editing_ = false;
    }

    if (column == SIZE_COL) {
      bool success;
      double size = item->text(SIZE_COL).toDouble(&success);
      if (success) {
        ann->Object->size = size;
      } else {
        item->setText(SIZE_COL, QString::number(ann->Object->size));
      }
    }

    om::event::Redraw2d();
    om::event::Redraw3d();
  }

 protected:
  void keyReleaseEvent(QKeyEvent* event) {
    if (!editing_ &&
        (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)) {
      QTreeWidgetItem* selectedItem = getSelected();
      if (selectedItem) {
        managedAnnotation* selected = getAnnotation(selectedItem);
        if (!selected) {
          return;
        }
        om::annotation::manager* manager = getManager(selectedItem);
        if (!manager) {
          return;
        }
        manager->Remove(selected->ID);
        delete selectedItem;
        om::event::Redraw2d();
        om::event::Redraw3d();
        return;
      }
    }

    if (editing_ && event->key() == Qt::Key_Escape) {
      editing_ = false;
    }

    QTreeWidget::keyReleaseEvent(event);
  }

 private:
  void highlight(QTreeWidgetItem* item) {
    managedAnnotation* annotation = getAnnotation(item);
    if (!annotation) {
      return;
    }

    vgs_.View2dState().SetScaledSliceDepth(
        annotation->Object->coord.ToGlobal());
    om::event::ViewCenterChanged();
    om::event::View3dRecenter();
  }

  static managedAnnotation* getAnnotation(QTreeWidgetItem* item) {
    QVariant data = item->data(POSITION_COL, Qt::UserRole);
    void* ptr = data.value<void*>();
    return (managedAnnotation*)ptr;
  }

  static om::annotation::manager* getManager(QTreeWidgetItem* item) {
    return static_cast<om::annotation::manager*>(
        item->data(TEXT_COL, Qt::UserRole).value<void*>());
  }

  QTreeWidgetItem* getSelected() {
    QList<QTreeWidgetItem*> items = selectedItems();
    if (items.length() > 0) {
      return items[0];
    }
    return nullptr;
  }

  OmMipVolume* getVol() {
    {
      const ChannelDataWrapper cdw = vgs_.Channel();
      if (cdw.IsValidWrapper()) {
        return cdw.GetChannel();
      }
    }

    {
      const SegmentationDataWrapper sdw = vgs_.Segmentation();
      if (sdw.IsValidWrapper()) {
        return sdw.GetSegmentation();
      }
    }

    return nullptr;
  }

  void setLocationText(QTreeWidgetItem* row, const om::annotation::data& a) {
    coords::Global c = a.coord.ToGlobal();
    std::stringstream ss;
    ss << c.x << ", " << c.y << ", " << c.z;
    row->setText(POSITION_COL, QString::fromStdString(ss.str()));
  }

  OmViewGroupState& vgs_;
  bool editing_;

  static const int ENABLE_COL = 0;
  static const int COLOR_COL = 1;
  static const int TEXT_COL = 2;
  static const int POSITION_COL = 3;
  static const int SIZE_COL = 4;
};

}  // namespace sidebars
}  // namespace om
