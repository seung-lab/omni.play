#ifndef OM_GLOBAL_KEY_PRESS_H
#define OM_GLOBAL_KEY_PRESS_H

#include "common/omCommon.h"
#include "gui/toolbars/toolbarManager.h"
#include "segment/omSegmentSelected.hpp"
#include "system/omAppState.hpp"
#include "system/omConnect.hpp"
#include "system/omStateManager.h"
#include "viewGroup/omBrushSize.hpp"

#include <QKeyEvent>

class OmGlobalKeyPress : public QWidget {
Q_OBJECT

private:
    QWidget *const parent_;

    boost::scoped_ptr<QShortcut> b_;
    boost::scoped_ptr<QShortcut> comma_;
    boost::scoped_ptr<QShortcut> greater_;
    boost::scoped_ptr<QShortcut> less_;
    boost::scoped_ptr<QShortcut> m_;
    boost::scoped_ptr<QShortcut> n_;
    boost::scoped_ptr<QShortcut> r_;
    boost::scoped_ptr<QShortcut> v_;

    void setShortcut(boost::scoped_ptr<QShortcut>& shortcut,
                     const QKeySequence key,
                     const char* method)
    {
        shortcut.reset(new QShortcut(parent_));
        shortcut->setKey(key);
        shortcut->setContext(Qt::ApplicationShortcut);

        om::connect(shortcut.get(), SIGNAL(activated()),
                    this, method);
    }

    void setTool(const om::tool::mode tool)
    {
        ToolBarManager* tbm = OmAppState::GetToolBarManager();
        if(tbm){
            tbm->SetTool(tool);
        }
    }

private Q_SLOTS:
    void keyV(){
        setTool(om::tool::CROSSHAIR);
    }

    void keyB(){
        setTool(om::tool::PAN);
    }

    void keyN(){
        setTool(om::tool::SELECT);
    }

    void keyM(){
        setTool(om::tool::PAINT);
    }

    void keyComma(){
        setTool(om::tool::ERASE);
    }

    void keyR(){
        OmSegmentSelected::RandomizeColor();
    }

    void keyLess()
    {
        OmStateManager::BrushSize()->DecreaseSize();
        OmEvents::Redraw2d();
    }

    void keyGreater()
    {
        OmStateManager::BrushSize()->IncreaseSize();
        OmEvents::Redraw2d();
    }

public:
    OmGlobalKeyPress(QWidget* parent)
        : QWidget(parent)
        , parent_(parent)
    {
        setShortcut(b_,       QKeySequence(Qt::Key_B),       SLOT(keyB()));
        setShortcut(comma_,   QKeySequence(Qt::Key_Comma),   SLOT(keyComma()));
        setShortcut(greater_, QKeySequence(Qt::Key_Greater), SLOT(keyGreater()));
        setShortcut(less_,    QKeySequence(Qt::Key_Less),    SLOT(keyLess()));
        setShortcut(m_,       QKeySequence(Qt::Key_M),       SLOT(keyM()));
        setShortcut(n_,       QKeySequence(Qt::Key_N),       SLOT(keyN()));
        setShortcut(r_,       QKeySequence(Qt::Key_R),       SLOT(keyR()));
        setShortcut(v_,       QKeySequence(Qt::Key_V),       SLOT(keyV()));
    }
};

#endif
