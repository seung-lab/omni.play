#ifndef SHOW_PAINT_TOOLS_HPP
#define SHOW_PAINT_TOOLS_HPP

#include "gui/widgets/omButton.hpp"
#include "gui/brushToolbox/brushToolbox.hpp"

class ShowPaintTools : public OmButton<QWidget> {
private:
    BrushToolbox* brushToolbox_;

public:
    ShowPaintTools(QWidget* parent)
        : OmButton<QWidget>(parent, "", "Show brush tools", true)
        , brushToolbox_(new BrushToolbox(parent))
    {
        SetIcon(":/toolbars/mainToolbar/icons/1301504581_emblem-art2.png");
    }

private:
    void doAction()
    {
        if(!isChecked()) {
            brushToolbox_->Show();

        } else {
            brushToolbox_->Hide();
        }
    }

};

#endif
