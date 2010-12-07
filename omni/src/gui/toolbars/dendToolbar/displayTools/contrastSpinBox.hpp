#ifndef CONTRAST_SPIN_BOX_HPP
#define CONTRAST_SPIN_BOX_HPP

#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/displayTools/displayTools.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "system/omProjectData.h"
#include "tiles/cache/omTileCache.h"
#include "utility/image/omFilterImage.hpp"
#include "viewGroup/omViewGroupState.h"

#include <limits>

class ContrastSpinBox : public OmDoubleSpinBox {
Q_OBJECT

public:
	ContrastSpinBox(DisplayTools* d)
		: OmDoubleSpinBox(d, om::UPDATE_AS_TYPE)
		, mParent(d)
	{
		setSingleStep(0.05);
		setRange(-5, 5);
		setInitialGUIThresholdValue();
	}

private:
	DisplayTools *const mParent;

	void actUponThresholdChange(float threshold)
	{
		OmImageFilter::SetContrastValue(threshold);
		OmTileCache::ClearChannel();
		std::cout << "contrast changed to " << threshold << "\n";
	}

	void setInitialGUIThresholdValue()
	{
        setValue(OmImageFilter::GetContrastValue());
	}

	OmViewGroupState* vgs() const {
		return mParent->getViewGroupState();
	}
};

#endif
