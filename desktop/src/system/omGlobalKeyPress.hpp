#pragma once

#include "common/common.h"
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
    boost::scoped_ptr<QShortcut> j_;
    boost::scoped_ptr<QShortcut> less_;
    boost::scoped_ptr<QShortcut> m_;
    boost::scoped_ptr<QShortcut> n_;
    boost::scoped_ptr<QShortcut> period_;
    boost::scoped_ptr<QShortcut> r_;
    boost::scoped_ptr<QShortcut> k_;
    boost::scoped_ptr<QShortcut> l_;
    boost::scoped_ptr<QShortcut> slash_;


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

    void keyPeriod(){
        setTool(om::tool::FILL);
    }

    void keyK(){
        setTool(om::tool::CUT);
    }

    void keyL(){
        setTool(om::tool::LANDMARK);
    }

    // void keySlash(){
    // 	setTool(om::tool::KALINA);
    // }

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

    void keyJ()
    {
        const OmIDsSet& segset = SegmentationDataWrapper::ValidIDs();

        FOR_EACH(iter, segset){
            OmActions::JoinSegments(SegmentationDataWrapper(*iter));
        }

        OmEvents::Redraw2d();
        OmEvents::Redraw3d();
    }

public:
    OmGlobalKeyPress(QWidget* parent)
        : QWidget(parent)
        , parent_(parent)
    {
        setShortcut(b_,       QKeySequence(Qt::Key_B),       SLOT(keyB()));
        setShortcut(j_,       QKeySequence(Qt::Key_J),       SLOT(keyJ()));
        setShortcut(comma_,   QKeySequence(Qt::Key_Comma),   SLOT(keyComma()));
        setShortcut(greater_, QKeySequence(Qt::Key_Greater), SLOT(keyGreater()));
        setShortcut(less_,    QKeySequence(Qt::Key_Less),    SLOT(keyLess()));
        setShortcut(m_,       QKeySequence(Qt::Key_M),       SLOT(keyM()));
        setShortcut(n_,       QKeySequence(Qt::Key_N),       SLOT(keyN()));
        setShortcut(period_,  QKeySequence(Qt::Key_Period),  SLOT(keyPeriod()));
        setShortcut(r_,       QKeySequence(Qt::Key_R),       SLOT(keyR()));
        setShortcut(k_,       QKeySequence(Qt::Key_K),       SLOT(keyK()));
        setShortcut(l_,       QKeySequence(Qt::Key_L),       SLOT(keyL()));
        // setShortcut(slash_,   QKeySequence(Qt::Key_Slash),   SLOT(keySlash()));
    }
};

