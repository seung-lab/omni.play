#ifndef MESH_PREVIEW_FACTOR_LINEEDIT_HPP
#define MESH_PREVIEW_FACTOR_LINEEDIT_HPP

#include "common/omCommon.h"
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
			OmMeshParams::SetDownScallingFactor(om::StrToNum<int>(text()));
		} catch (...){
		}
	}
};

#endif

