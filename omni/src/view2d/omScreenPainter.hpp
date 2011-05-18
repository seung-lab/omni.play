#pragma once

#include "events/omEvents.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "view2d/om2dPreferences.hpp"
#include "view2d/omDisplayInfo.hpp"
#include "view2d/omView2dCore.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omBrushSize.hpp"

#include <QPainter>

class OmScreenPainter{
public:
    OmScreenPainter(OmView2dCore* v2d, OmView2dState* state)
        : v2d_(v2d)
        , state_(state)
        , viewType_(v2d->GetViewType())
        , shouldDisplayInfo_(OmPreferences::GetBoolean(om::PREF_VIEW2D_SHOW_INFO_BOOL))
    {}

    void PaintExtras()
    {
        toolMode_ = OmStateManager::GetToolMode();
        mousePoint_ = state_->GetMousePoint();

        QPainter painter(v2d_);

        QPen pen;
        pen.setColor(getPenColor());
        painter.setPen(pen);

        if(amFilling())
        {
            drawFillTool(painter);

        } else if(showBrushSize())
        {
            if(amErasing())
            {
                painter.setPen(Qt::DotLine);
                showBrush(painter);
                painter.setPen(Qt::SolidLine);

            } else {
                showBrush(painter);
            }
        }

        if(shouldDisplayInfo_){
            displayInformation(painter, pen);
        }

        drawBoundingBox(painter);

        if(Om2dPreferences::ShowCrosshairs()){
            drawCursors(painter);
        }
    }

private:
    OmView2dCore *const v2d_;
    OmView2dState *const state_;

    const ViewType viewType_;
    const bool shouldDisplayInfo_;

    // current state
    om::tool::mode toolMode_;
    Vector2i mousePoint_;

    inline bool amFilling(){
        return om::tool::FILL == toolMode_;
    }

    inline bool amErasing(){
        return om::tool::ERASE == toolMode_;
    }

    void drawFillTool(QPainter& painter)
    {
        painter.drawRoundedRect(QRect(mousePoint_.x,
                                      mousePoint_.y,
                                      20,
                                      20),
                                5, 5);
    }

    void drawBoundingBox(QPainter& painter)
    {
        const Vector4i& vp = state_->getTotalViewport();
        painter.drawRect(vp.lowerLeftX,
                         vp.lowerLeftY,
                         vp.width - 1,
                         vp.height - 1);
    }

    void showBrush(QPainter& painter)
    {
        const int brushDiamater = state_->getZoomScale() * state_->getBrushSize()->Diameter();
        const double offset = 0.5 * brushDiamater;

        painter.drawEllipse(QRectF(mousePoint_.x - offset,
                                   mousePoint_.y - offset,
                                   brushDiamater,
                                   brushDiamater));
    }

    QColor getPenColor() const
    {
        switch(viewType_) {
        case XY_VIEW:
            return QColor(Qt::blue);
        case XZ_VIEW:
            return QColor(Qt::green);
        case ZY_VIEW:
            return QColor(Qt::red);
        default:
            throw OmArgException("invalid view type");
        }
    }

    void displayInformation(QPainter& painter, QPen& pen)
    {
        int yTop = 45;

        const int xoffset = 10;
        const int yTopOfText = state_->getTotalViewport().height - yTop;

        OmDisplayInfo di(painter, pen, yTopOfText, xoffset);

        if(state_->IsLevelLocked()){
            di.paint("MIP Level Locked (Press L to unlock.)");
        }

        di.paint(state_->getMipLevel(), "Level");

        di.paint(state_->getZoomScale(), "zoomFactor", 2);

        di.paint(depthString());

        printTileCount(di);
    }

    QString depthString()
    {
        const int dataDepth = state_->Location()->DataDepth();

        const int scaledDepth = state_->Location()->ScaledDepth();

        // TODO: remove: hack for abs coords
        const Vector3i absOffsetVec = state_->getVol()->Coords().GetAbsOffset();
        const int absOffset = OmView2dConverters::GetViewTypeDepth(absOffsetVec, state_->getViewType());

        QString depthStr = QString::number(scaledDepth + absOffset) + " Slice Depth";
        if(scaledDepth != dataDepth){
            depthStr += " (" + QString::number(dataDepth) + " data)";
        }

        return depthStr;
    }

    void printTileCount(OmDisplayInfo& di)
    {
        const int tileCountIncomplete = v2d_->GetTileCountIncomplete();
        const int tileCount = v2d_->GetTileCount();

        if(tileCountIncomplete)
        {
            di.paint(tileCountIncomplete, "tiles incomplete of",
                     tileCount, "tiles");

        }else{
            di.paint(tileCount, "tiles");
        }
    }

    void drawCursors(QPainter& painter)
    {
        const Vector4i& vp = state_->getTotalViewport();
        const int fullHeight = vp.height;
        const int halfHeight = fullHeight/2;
        const int fullWidth = vp.width;
        const int halfWidth = fullWidth/2;
        const int crosshairHoleSize = Om2dPreferences::CrosshairHoleSize();

        std::pair<QColor, QColor> colors = getCursorColors();

        painter.setPen(colors.first);

        painter.drawLine(halfWidth, 0,
                         halfWidth, halfHeight - crosshairHoleSize);
        painter.drawLine(halfWidth, halfHeight + crosshairHoleSize,
                         halfWidth, fullHeight);

        painter.setPen(colors.second);

        painter.drawLine(0, halfHeight,
                         halfWidth - crosshairHoleSize, halfHeight);
        painter.drawLine(fullWidth, halfHeight,
                         halfWidth + crosshairHoleSize, halfHeight);
    }

    std::pair<QColor, QColor> getCursorColors() const
    {
        switch (viewType_) {
        case XY_VIEW:
            return std::make_pair(Qt::red, Qt::green);
        case XZ_VIEW:
            return std::make_pair(Qt::red, Qt::blue);
        case ZY_VIEW:
            return std::make_pair(Qt::blue, Qt::green);
        default:
            throw OmArgException("invalid viewtype");
        }
    }

    bool showBrushSize() const
    {
        switch(toolMode_){
        case om::tool::PAINT:
        case om::tool::ERASE:
        case om::tool::FILL:
        case om::tool::SELECT:
            return true;
        default:
            return false;
        }
    }
};

