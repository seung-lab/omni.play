#pragma once

#include "zi/omUtility.h"

#include <boost/scoped_ptr.hpp>
#include <QWidget>

class QString;
class OmViewGroupState;
class ElementListBoxImpl;
class SegmentDataWrapper;
class SegmentationDataWrapper;

class ElementListBox : private om::singletonBase<ElementListBox> {
private:
    std::unique_ptr<ElementListBoxImpl> impl_;

public:
    static QWidget* Widget();

    static void Create(OmViewGroupState* vgs);
    static void Delete();
    static void Reset();

    static void SetActiveTab(QWidget* tab);
    static void SetTitle(const QString& title);
    static void AddTab(const int preferredIndex, QWidget* tab,
                       const QString& tabTitle);

    static void PopulateLists();
    static void RebuildLists(const SegmentDataWrapper& sdw);
    static void UpdateSegmentListBox(const SegmentationDataWrapper& sdw);

private:
    ElementListBox();
    ~ElementListBox();

    friend class zi::singleton<ElementListBox>;
};

