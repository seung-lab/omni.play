#pragma once

#include "system/omConnect.hpp"

#include <QListWidget>

namespace om {
namespace channelInspector {

// based off http://doc.qt.nokia.com/latest/dialogs-configdialog-configdialog-cpp.html

class PageLink : public QListWidgetItem {
public:
    PageLink(QListWidget* parent, const QString& title, const QString iconPath)
        : QListWidgetItem(parent)
    {
        setIcon(QIcon(iconPath));
        setText(title);
        setTextAlignment(Qt::AlignHCenter);
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
};

class PageSelector : public QListWidget {
Q_OBJECT

public:
    PageSelector(QWidget* parent)
        : QListWidget(parent)
    {
        setViewMode(QListView::IconMode);
        setIconSize(QSize(96, 84));
        setMovement(QListView::Static);
        setMaximumWidth(128);
        setSpacing(12);

        addPageLink("Builder", "1302814771_advancedsettings.png");
        addPageLink("Metadata", "1305228725_package_graphics.png");
        addPageLink("Export", "1302822648_filesaveas.png");

        setCurrentRow(0);

        om::connect(this, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
                    this, SLOT(changePage(QListWidgetItem*, QListWidgetItem*)));
    }

Q_SIGNALS:
    void RowChanged(int row);

private Q_SLOTS:
    void changePage(QListWidgetItem* current, QListWidgetItem* previous)
    {
        if (!current){
            current = previous;
        }

        RowChanged(row(current));
    }

private:
    void addPageLink(const QString& title, const QString& iconName){
        new PageLink(this, title, ":/inspectors/channel/icons/" + iconName);
    }
};

} // namespace channelInspector
} // namespace om

