#include "events/details/omSegmentEvent.h"
#include "gui/brushToolbox/brushToolboxImpl.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "gui/widgets/omButton.hpp"
#include "gui/widgets/omIntSpinBox.hpp"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegments.h"
#include "segment/omSegmentUtils.hpp"
#include "system/omStateManager.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omBrushSize.hpp"

#include <limits>

class BrushColor : public OmButton<QWidget>, public OmSegmentEventListener {
public:
    BrushColor(QWidget* d)
        : OmButton<QWidget>(d,
                            "",
                            "Brush Color",
                            false)
    {
        update();
    }

private:
    void doAction()
    {}

    void SegmentModificationEvent(OmSegmentEvent*){}
    void SegmentGUIlistEvent(OmSegmentEvent*){}
    void SegmentSelectedEvent(OmSegmentEvent*){
        update();
    }

    void update()
    {
        SegmentDataWrapper sdwUnknownDepth =
            OmSegmentSelected::GetSegmentForPainting();

        if(!sdwUnknownDepth.IsValidWrapper()){
            return;
        }

        SegmentDataWrapper sdw(sdwUnknownDepth.FindRoot());

        const QPixmap pixmap = OmSegmentUtils::SegColorAsQPixmap(sdw);

        setIcon(QIcon(pixmap));
    }
};

class AddSegmentButton : public OmButton<QWidget> {
public:
    AddSegmentButton(QWidget* d)
        : OmButton<QWidget>(d,
                            "Add Segment",
                            "Add Segment",
                            false)
    {}

private:
    void doAction()
    {
        const SegmentationDataWrapper sdw(1);

        OmSegment* newSeg = sdw.Segments()->AddSegment();

        // save new segment IDs; if we don't do this, and the user paints,
        //  the segment metadata will be out of sync with the data on disk...
        OmActions::Save();

        ElementListBox::RebuildLists(SegmentDataWrapper(newSeg));

        OmSegmentSelector sel(sdw, this, "addSegmentButton" );
        sel.selectJustThisSegment(newSeg->value(), true);
        sel.sendEvent();
    }
};

class OmBrushSizeSpinBox : public OmIntSpinBox {
private:
    virtual void setInitialGUIThresholdValue(){
        setValue(OmStateManager::BrushSize()->Diameter());
    }

    virtual void actUponSpinboxChange(const int value)
    {
        OmStateManager::BrushSize()->SetDiameter(value);
        OmEvents::Redraw2d();
    }

public:
    OmBrushSizeSpinBox(QWidget* parent)
        : OmIntSpinBox(parent, om::UPDATE_AS_TYPE)
    {
        setValue(OmStateManager::BrushSize()->Diameter());
        setSingleStep(1);
        setMinimum(1);
        setMaximum(std::numeric_limits<int32_t>::max());

        om::connect(OmStateManager::BrushSize(), SIGNAL(SignalBrushSizeChange(int)),
                    this, SLOT(setValue(int)));
    }
};

BrushToolboxImpl::BrushToolboxImpl(QWidget* parent)
    : QDialog(parent, Qt::Tool)
{
    setAttribute(Qt::WA_ShowWithoutActivating);

    QVBoxLayout* layout = new QVBoxLayout;

    layout->addWidget(new BrushColor(this));
    layout->addWidget(new OmBrushSizeSpinBox(this));
    layout->addWidget(new AddSegmentButton(this));

    setLayout(layout);
}
