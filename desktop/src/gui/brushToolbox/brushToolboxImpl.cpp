#include "events/details/omSegmentEvent.h"
#include "gui/brushToolbox/brushToolboxImpl.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "gui/widgets/omButton.hpp"
#include "gui/widgets/omIntSpinBox.hpp"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegmentUtils.hpp"
#include "segment/omSegments.h"
#include "system/omStateManager.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omBrushSize.hpp"
#include "viewGroup/omViewGroupState.h"

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
        SegmentDataWrapper sdwUnknownDepth = OmSegmentSelected::GetSegmentForPainting();

        QPixmap pixmap;

        if(sdwUnknownDepth.IsValidWrapper())
        {
            SegmentDataWrapper sdw = SegmentDataWrapper(sdwUnknownDepth.FindRoot());
            pixmap = om::utils::color::OmColorAsQPixmap(sdw.GetColorInt());

        } else {
            OmColor black = { 0, 0, 0 };
            pixmap = om::utils::color::OmColorAsQPixmap(black);
        }

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

class SetBlackColorButton : public OmButton<QWidget> {
public:
    SetBlackColorButton(QWidget* d, OmViewGroupState* vgs)
        : OmButton<QWidget>(d,
                            "Set No Segment for Color",
                            "Set No Segment for Color",
                            false)
        , vgs_(vgs)
    {}

private:
    OmViewGroupState *const vgs_;

    void doAction()
    {
        SegmentDataWrapper sdw(vgs_->Segmentation().GetID(), 0);
        OmSegmentSelected::SetSegmentForPainting(sdw);
    }
};

BrushToolboxImpl::BrushToolboxImpl(QWidget* parent, OmViewGroupState* vgs)
    : QDialog(parent, Qt::Tool)
{
    setAttribute(Qt::WA_ShowWithoutActivating);

    QVBoxLayout* layout = new QVBoxLayout;

    layout->addWidget(new BrushColor(this));
    layout->addWidget(new OmBrushSizeSpinBox(this));
    layout->addWidget(new AddSegmentButton(this));
    layout->addWidget(new SetBlackColorButton(this, vgs));

    setLayout(layout);
}
