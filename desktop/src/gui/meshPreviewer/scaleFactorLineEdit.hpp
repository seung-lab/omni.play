#pragma once

#include "common/common.h"
#include "gui/widgets/omLineEdit.hpp"
#include "mesh/omMeshParams.hpp"

class MeshScaleFactorLineEdit : private OmLineEdit {
public:
    MeshScaleFactorLineEdit(QWidget* widget)
        : OmLineEdit(widget)
    {
        setText(QString::number(OmMeshParams::GetDownScallingFactor()));
    }

private:
    void doUpdateFromEditDoneSignel()
    {
        try {
            const int factor = om::string::toNum<int>(text().toStdString());
            OmMeshParams::SetDownScallingFactor(factor);
        } catch (...){
        }
    }
};

