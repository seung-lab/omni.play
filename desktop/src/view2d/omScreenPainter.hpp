#pragma once

#include "events/omEvents.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "view2d/om2dPreferences.hpp"
#include "view2d/omDisplayInfo.hpp"
#include "view2d/omView2dCore.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omBrushSize.hpp"
#include "annotation/annotation.h"

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

        if(om::tool::LANDMARK == toolMode_){
            // showLandmark(painter);
        }

        if(shouldDisplayInfo_){
            displayInformation(painter, pen);
        }

        drawBoundingBox(painter);

        if(Om2dPreferences::ShowCrosshairs()){
            drawCursors(painter);
        }

        if(state_->getViewGroupState()->getAnnotationVisible()) {
            drawAnnotations(painter);
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
        const Vector4i& vp = state_->Coords().getTotalViewport();
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
            return QColor(Qt::white);
        case XZ_VIEW:
            return QColor(Qt::white);
        case ZY_VIEW:
            return QColor(Qt::white);
        default:
            throw OmArgException("invalid view type");
        }
    }

    void displayInformation(QPainter& painter, QPen& pen)
    {
        int yTop = 45;

        const int xoffset = 10;
        const int yTopOfText = state_->Coords().getTotalViewport().height - yTop;

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
    	const om::coords::Global global = state_->Location();
        const om::coords::Data data = global.
            toDataCoord(state_->getVol(), state_->getMipLevel());

		const int globalDepth = state_->getViewTypeDepth(global);
        const int dataDepth = state_->getViewTypeDepth(data);

        QString depthStr = QString::number(globalDepth) + " - " + QString::number(dataDepth);

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
        const Vector4i& vp = state_->Coords().getTotalViewport();
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
            return std::make_pair(Qt::green, Qt::red);
        case XZ_VIEW:
            return std::make_pair(Qt::blue, Qt::red);
        case ZY_VIEW:
            return std::make_pair(Qt::green, Qt::blue);
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

    void showLandmark(QPainter& painter)
    {
        const QImage star(":/toolbars/mainToolbar/icons/1308021634_keditbookmarks.png");

        const QPoint point(mousePoint_.x, mousePoint_.y);

        painter.drawImage(point, star);
    }

    void drawAnnotations(QPainter& painter)
    {
        FOR_EACH(i, SegmentationDataWrapper::ValidIDs())
        {
            SegmentationDataWrapper sdw(*i);

            om::annotation::manager &annotations = *sdw.GetSegmentation().Annotations();

            FOR_EACH(it, annotations.Enabled())
            {
            	om::annotation::data& a = *it->Object;

                if(!closeInDepth(a.coord.toGlobalCoord()))
                    continue;

                om::screenCoord loc = a.coord.toGlobalCoord().toScreenCoord(state_);

                QPen pen;
                pen.setColor(QColor::fromRgb(a.color.red, a.color.green, a.color.blue));
                painter.setPen(pen);

                OmDisplayInfo di(painter, pen, loc.y, loc.x);
                di.paint(QString::fromStdString(a.comment));
            }
        }
    }

    static const int DEPTH_CUTOFF = 20;

    bool closeInDepth(om::coords::Global point)
    {
        int depth = state_->getViewTypeDepth(point);
        int plane = state_->getViewTypeDepth(state_->Location());

        return abs(depth - plane) < DEPTH_CUTOFF;
    }
};

