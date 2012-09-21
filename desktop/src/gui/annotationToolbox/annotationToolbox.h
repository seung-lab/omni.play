#pragma once

#include <QDialog>
#include <QtGui>
#include "gui/widgets/omButton.hpp"
#include "utility/color.hpp"
#include "viewGroup/omViewGroupState.h"
#include "system/omConnect.hpp"

namespace om {
namespace gui {

class ColorButton : public OmButton<QWidget>
{
    private:
        OmViewGroupState *vgs_;
        QColor cur_;

    public:
        ColorButton(QWidget* d, OmViewGroupState *vgs)
        : OmButton<QWidget>(d, "", "Choose a Color", false)
        , vgs_(vgs)
        , cur_(QColor(255,255,255))
        {
            updateColor();
        }

    private:
        void updateColor()
        {
            vgs_->setAnnotationColor(om::utils::color::QColorToOmColor(cur_));

            const QPixmap pixm =  om::utils::color::MakeQPixmap(cur_);
            setIcon(QIcon(pixm));

            update();
        }

        void doAction()
        {
            QColor color = QColorDialog::getColor(cur_, this);

            if (!color.isValid()) {
                return;
            }

            cur_ = color;
            updateColor();
        }
};

class AnnotationLineEdit : public QLineEdit
{
    Q_OBJECT

    private:
        OmViewGroupState* vgs_;

    private Q_SLOTS:
        void update(const QString &text) {
            vgs_->setAnnotationString(text.toStdString());
        }

    public:
        AnnotationLineEdit(QWidget* d, OmViewGroupState* vgs)
        : QLineEdit(d)
        , vgs_(vgs)
        {
            om::connect(this, SIGNAL(textChanged(const QString&)),
                        this, SLOT(update(const QString&)));
        }
};

class AnnotationToolbox : public QDialog {
    public:
        AnnotationToolbox(QWidget* parent, OmViewGroupState* vgs);

        virtual ~AnnotationToolbox()
        {}
};

} // namespace gui
} // namespace om
